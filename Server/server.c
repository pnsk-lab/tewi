/* $Id$ */

#define SOURCE

#include "tw_server.h"

#include "tw_ssl.h"
#include "tw_config.h"
#include "tw_http.h"
#include "tw_module.h"
#include "tw_version.h"

#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>

#include <cm_string.h>
#include <cm_log.h>
#include <cm_dir.h>

#ifdef __MINGW32__
#include <winsock2.h>
#include <process.h>

#include "strptime.h"
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

extern struct tw_config config;
extern char tw_server[];

fd_set fdset;
int sockcount = 0;

SOCKADDR addresses[MAX_PORTS];
int sockets[MAX_PORTS];

/* https://qiita.com/gyu-don/items/5a640c6d2252a860c8cd */
int tw_wildcard_match(const char* wildcard, const char* target) {
	const char *pw = wildcard, *pt = target;

	while(1) {
		if(*pt == 0) {
			while(*pw == '*') pw++;
			return *pw == 0;
		} else if(*pw == 0) {
			return 0;
		} else if(*pw == '*') {
			return *(pw + 1) == 0 || tw_wildcard_match(pw, pt + 1) || tw_wildcard_match(pw + 1, pt);
		} else if(*pw == '?' || (*pw == *pt)) {
			pw++;
			pt++;
			continue;
		} else {
			return 0;
		}
	}
}

void close_socket(int sock) {
#ifdef __MINGW32__
	closesocket(sock);
#else
	close(sock);
#endif
}

int tw_server_init(void) {
	int i;
#ifdef __MINGW32__
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
#endif
	for(i = 0; config.ports[i] != -1; i++)
		;
	sockcount = i;
	for(i = 0; config.ports[i] != -1; i++) {
#ifdef NO_IPV6
		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
		int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
#endif
#ifdef __MINGW32__
		if(sock == INVALID_SOCKET)
#else
		if(sock < 0)
#endif
		{
			cm_log("Server", "Socket creation failure");
			return 1;
		}
		int yes = 1;
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0) {
			close_socket(sock);
			cm_log("Server", "setsockopt failure (reuseaddr)");
			return 1;
		}
		if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&yes, sizeof(yes)) < 0) {
			close_socket(sock);
			cm_log("Server", "setsockopt failure (nodelay)");
			return 1;
		}
#ifndef NO_IPV6
		int no = 0;
		if(setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&no, sizeof(no)) < 0) {
			close_socket(sock);
			cm_log("Server", "setsockopt failure (IPv6)");
			return 1;
		}
#endif
		memset(&addresses[i], 0, sizeof(addresses[i]));
#ifdef NO_IPV6
		addresses[i].sin_family = AF_INET;
		addresses[i].sin_addr.s_addr = INADDR_ANY;
		addresses[i].sin_port = htons(config.ports[i]);
#else
		addresses[i].sin6_family = AF_INET6;
		addresses[i].sin6_addr = in6addr_any;
		addresses[i].sin6_port = htons(config.ports[i]);
#endif
		if(bind(sock, (struct sockaddr*)&addresses[i], sizeof(addresses[i])) < 0) {
			close_socket(sock);
			cm_log("Server", "Bind failure");
			return 1;
		}
		if(listen(sock, 128) < 0) {
			close_socket(sock);
			cm_log("Server", "Listen failure");
			return 1;
		}
		sockets[i] = sock;
	}
	return 0;
}

size_t tw_read(SSL* ssl, int s, void* data, size_t len) {
	if(ssl == NULL) {
		return recv(s, data, len, 0);
	} else {
		return SSL_read(ssl, data, len);
	}
}

size_t tw_write(SSL* ssl, int s, void* data, size_t len) {
	if(ssl == NULL) {
		return send(s, data, len, 0);
	} else {
		return SSL_write(ssl, data, len);
	}
}

#define ERROR_HTML \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" \
	"<html>\n" \
	"	<head>\n" \
	"		<title>%s</title>\n" \
	"	</head>\n" \
	"	<body>\n" \
	"		<h1>%s</h1>\n" \
	"		<hr>\n" \
	"		", \
	    address, \
	    "\n" \
	    "	</body>\n" \
	    "</html>\n"

void _tw_process_page(SSL* ssl, int sock, const char* status, const char* type, FILE* f, const unsigned char* doc, size_t size, char** headers, time_t mtime, time_t cmtime) {
	char construct[512];
	if(mtime != 0 && cmtime != 0 && mtime <= cmtime) {
		status = "304 Not Modified";
		type = NULL;
		size = 0;
		headers = NULL;
		f = NULL;
		doc = NULL;
	}
	sprintf(construct, "%llu", (unsigned long long)size);
	tw_write(ssl, sock, "HTTP/1.1 ", 9);
	tw_write(ssl, sock, (char*)status, strlen(status));
	tw_write(ssl, sock, "\r\n", 2);
	if(type != NULL) {
		tw_write(ssl, sock, "Content-Type: ", 7 + 5 + 2);
		tw_write(ssl, sock, (char*)type, strlen(type));
		tw_write(ssl, sock, "\r\n", 2);
	}
	tw_write(ssl, sock, "Server: ", 6 + 2);
	tw_write(ssl, sock, tw_server, strlen(tw_server));
	tw_write(ssl, sock, "\r\n", 2);
	if(size != 0) {
		tw_write(ssl, sock, "Content-Length: ", 7 + 7 + 2);
		tw_write(ssl, sock, construct, strlen(construct));
		tw_write(ssl, sock, "\r\n", 2);
		if(mtime != 0) {
			struct tm* tm = gmtime(&mtime);
			char date[513];
			strftime(date, 512, "%a, %d %b %Y %H:%M:%S GMT", tm);
			tw_write(ssl, sock, "Last-Modified: ", 5 + 8 + 2);
			tw_write(ssl, sock, date, strlen(date));
			tw_write(ssl, sock, "\r\n", 2);
		}
	}
	int i;
	if(headers != NULL) {
		for(i = 0; headers[i] != NULL; i += 2) {
			tw_write(ssl, sock, headers[i], strlen(headers[i]));
			tw_write(ssl, sock, ": ", 2);
			tw_write(ssl, sock, headers[i + 1], strlen(headers[i + 1]));
			tw_write(ssl, sock, "\r\n", 2);
		}
	}
	tw_write(ssl, sock, "\r\n", 2);
	if(doc == NULL && f == NULL) return;
	size_t incr = 0;
	while(1) {
		if(f != NULL) {
			char buffer[128];
			fread(buffer, size < 128 ? size : 128, 1, f);
			tw_write(ssl, sock, buffer, size < 128 ? size : 128);
		} else {
			tw_write(ssl, sock, (unsigned char*)doc + incr, size < 128 ? size : 128);
		}
		incr += 128;
		if(size <= 128) break;
		size -= 128;
	}
}

void tw_process_page(SSL* ssl, int sock, const char* status, const char* type, FILE* f, const unsigned char* doc, size_t size, time_t mtime, time_t cmtime) { _tw_process_page(ssl, sock, status, type, f, doc, size, NULL, mtime, cmtime); }

const char* tw_http_status(int code) {
	if(code == 200) {
		return "200 OK";
	} else if(code == 308) {
		return "308 Permanent Redirect";
	} else if(code == 400) {
		return "400 Bad Request";
	} else if(code == 401) {
		return "401 Unauthorized";
	} else if(code == 403) {
		return "403 Forbidden";
	} else if(code == 404) {
		return "404 Not Found";
	} else {
		return "400 Bad Request";
	}
}

char* tw_http_default_error(int code, char* name, int port) {
	char address[1024];
	sprintf(address, "<address>%s Server at %s Port %d</address>", tw_server, name, port);

	char* st = cm_strdup(tw_http_status(code));
	char* st2;
	int i;
	for(i = 0; st[i] != 0; i++) {
		if(st[i] == ' ') {
			st2 = cm_strdup(st + i + 1);
			break;
		}
	}
	char* buffer = malloc(4096);
	char* str = cm_strcat3(ERROR_HTML);
	sprintf(buffer, str, st, st2);
	free(str);
	free(st);
	return buffer;
}

void tw_http_error(SSL* ssl, int sock, int error, char* name, int port) {
	char* str = tw_http_default_error(error, name, port);
	tw_process_page(ssl, sock, tw_http_status(error), "text/html", NULL, str, strlen(str), 0, 0);
	free(str);
}

void addstring(char** str, const char* add, ...) {
	int i;
	char cbuf[2];
	cbuf[1] = 0;
	va_list va;
	va_start(va, add);
	for(i = 0; add[i] != 0; i++) {
		cbuf[0] = add[i];
		if(add[i] == '%') {
			i++;
			if(add[i] == 's') {
				char* tmp = *str;
				*str = cm_strcat(tmp, va_arg(va, const char*));
				free(tmp);
			} else if(add[i] == 'h') {
				char* h = cm_html_escape(va_arg(va, const char*));
				char* tmp = *str;
				*str = cm_strcat(tmp, h);
				free(tmp);
				free(h);
			} else if(add[i] == 'l') {
				char* h = cm_url_escape(va_arg(va, const char*));
				char* tmp = *str;
				*str = cm_strcat(tmp, h);
				free(tmp);
				free(h);
			} else if(add[i] == 'd') {
				int n = va_arg(va, int);
				char* h = malloc(512);
				sprintf(h, "%d", n);
				char* tmp = *str;
				*str = cm_strcat(tmp, h);
				free(tmp);
				free(h);
			} else if(add[i] == '%') {
				char* tmp = *str;
				*str = cm_strcat(tmp, "%");
				free(tmp);
			}
		} else {
			char* tmp = *str;
			*str = cm_strcat(tmp, cbuf);
			free(tmp);
		}
	}
}

char* tw_get_mime(const char* ext, struct tw_config_entry* vhost_entry) {
	char* mime = "application/octet-stream";
	if(ext == NULL) return mime;
	bool set = false;
	int i;
	for(i = 0; i < vhost_entry->mime_count; i++) {
		if(strcmp(vhost_entry->mimes[i].ext, "all") == 0 || (ext != NULL && tw_wildcard_match(vhost_entry->mimes[i].ext, ext))) {
			mime = vhost_entry->mimes[i].mime;
			set = true;
		}
	}
	if(!set) {
		for(i = 0; i < config.root.mime_count; i++) {
			if(strcmp(config.root.mimes[i].ext, "all") == 0 || (ext != NULL && tw_wildcard_match(config.root.mimes[i].ext, ext))) {
				mime = config.root.mimes[i].mime;
			}
		}
	}
	return mime;
}

char* tw_get_icon(const char* mime, struct tw_config_entry* vhost_entry) {
	char* icon = "";
	if(mime == NULL) return "";
	bool set = false;
	int i;
	for(i = 0; i < vhost_entry->icon_count; i++) {
		if(strcmp(vhost_entry->icons[i].mime, "all") == 0 || (mime != NULL && tw_wildcard_match(vhost_entry->icons[i].mime, mime))) {
			icon = vhost_entry->icons[i].icon;
			set = true;
		}
	}
	if(!set) {
		for(i = 0; i < config.root.icon_count; i++) {
			if(strcmp(config.root.icons[i].mime, "all") == 0 || (mime != NULL && tw_wildcard_match(config.root.icons[i].mime, mime))) {
				icon = config.root.icons[i].icon;
			}
		}
	}
	return icon;
}

#ifdef __MINGW32__
struct pass_entry {
	int sock;
	int port;
	bool ssl;
	SOCKADDR addr;
};

unsigned int WINAPI tw_server_pass(void* ptr) {
	int sock = ((struct pass_entry*)ptr)->sock;
	bool ssl = ((struct pass_entry*)ptr)->ssl;
	int port = ((struct pass_entry*)ptr)->port;
	SOCKADDR addr = ((struct pass_entry*)ptr)->addr;
	free(ptr);
#else
void tw_server_pass(int sock, bool ssl, int port, SOCKADDR addr) {
#endif
	char* name = config.hostname;

	SSL_CTX* ctx = NULL;
	SSL* s = NULL;
	bool sslworks = false;
	if(ssl) {
		ctx = tw_create_ssl_ctx(port);
		s = SSL_new(ctx);
		SSL_set_fd(s, sock);
		if(SSL_accept(s) <= 0) goto cleanup;
		sslworks = true;
	}
	struct tw_http_request req;
	struct tw_http_response res;
	struct tw_tool tools;
	res._processed = false;
	tw_init_tools(&tools);
	int ret = tw_http_parse(s, sock, &req);
	if(ret == 0) {
		char* vhost = cm_strdup(config.hostname);
		int i;
		time_t cmtime = 0;
		for(i = 0; req.headers[i] != NULL; i += 2) {
			if(cm_strcaseequ(req.headers[i], "Host")) {
				free(vhost);
				vhost = cm_strdup(req.headers[i + 1]);
			} else if(cm_strcaseequ(req.headers[i], "If-Modified-Since")) {
				struct tm tm;
				strptime(req.headers[i + 1], "%a, %d %b %Y %H:%M:%S GMT", &tm);
#ifdef __MINGW32__
				cmtime = _mkgmtime(&tm);
#else
				cmtime = timegm(&tm);
#endif
			}
		}
		cm_log("Server", "Host is %s", vhost);
		int port = s == NULL ? 80 : 443;
		char* host = cm_strdup(vhost);
		for(i = 0; vhost[i] != 0; i++) {
			if(vhost[i] == ':') {
				host[i] = 0;
				port = atoi(host + i + 1);
				break;
			}
		}
		cm_log("Server", "Hostname is `%s', port is `%d'", host, port);
		struct tw_config_entry* vhost_entry = tw_vhost_match(host, port);
		for(i = 0; i < config.module_count; i++) {
			tw_mod_request_t mod_req = (tw_mod_request_t)tw_module_symbol(config.modules[i], "mod_request");
			if(mod_req != NULL) {
				int ret = mod_req(&tools, &req, &res);
				int co = ret & 0xff;
				if(co == _TW_MODULE_PASS) continue;
				if(co == _TW_MODULE_STOP) {
					res._processed = true;
					break;
				}
				if(co == _TW_MODULE_ERROR) {
					tw_http_error(s, sock, (ret & 0xffff00) >> 8, name, port);
					break;
				}
			}
		}
		if(!res._processed) {
			cm_log("Server", "Document root is %s", vhost_entry->root == NULL ? "not set" : vhost_entry->root);
			char* path = cm_strcat(vhost_entry->root == NULL ? "" : vhost_entry->root, req.path);
			cm_log("Server", "Filesystem path is %s", path);
			struct stat st;
			if(stat(path, &st) == 0) {
				if(!tw_permission_allowed(path, addr, req, vhost_entry)) {
					tw_http_error(s, sock, 403, name, port);
				} else if(S_ISDIR(st.st_mode)) {
					if(req.path[strlen(req.path) - 1] != '/') {
						char* headers[3] = {"Location", cm_strcat(req.path, "/"), NULL};
						_tw_process_page(s, sock, tw_http_status(308), NULL, NULL, NULL, 0, headers, 0, 0);
						free(headers[1]);
					} else {
						char** indexes = vhost_entry->index_count == 0 ? config.root.indexes : vhost_entry->indexes;
						int index_count = vhost_entry->index_count == 0 ? config.root.index_count : vhost_entry->index_count;
						bool found = false;
						for(i = 0; i < index_count; i++) {
							char* p = cm_strcat3(path, "/", indexes[i]);
							FILE* f = fopen(p, "rb");
							if(f != NULL) {
								char* ext = NULL;
								int j;
								for(j = strlen(p) - 1; j >= 0; j--) {
									if(p[j] == '.') {
										ext = cm_strdup(p + j);
										break;
									} else if(p[j] == '/') {
										break;
									}
								}
								struct stat st;
								stat(p, &st);
								char* mime = tw_get_mime(ext, vhost_entry);
								tw_process_page(s, sock, tw_http_status(200), mime, f, NULL, st.st_size, 0, 0);
								fclose(f);
								free(p);
								found = true;
								break;
							}
							free(p);
						}
						if(!found) {
							char* str = malloc(1);
							str[0] = 0;
							char** items = cm_scandir(path);
							addstring(&str, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
							addstring(&str, "<html>\n");
							addstring(&str, "	<head>\n");
							addstring(&str, "		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
							addstring(&str, "		<title>Index of %h</title>\n", req.path);
							addstring(&str, "	</head>\n");
							addstring(&str, "	<body>\n");
							addstring(&str, "		<h1>Index of %h</h1>\n", req.path);
							addstring(&str, "		<hr>\n");
							addstring(&str, "		<table border=\"0\">\n");
							addstring(&str, "			<tr>\n");
							addstring(&str, "				<th></th>\n");
							addstring(&str, "				<th>Filename</th>\n");
							addstring(&str, "				<th>MIME</th>\n");
							addstring(&str, "				<th>Size</th>\n");
							addstring(&str, "			</tr>\n");
							int readme = -1;
							char** readmes = vhost_entry->readme_count == 0 ? config.root.readmes : vhost_entry->readmes;
							int readme_count = vhost_entry->readme_count == 0 ? config.root.readme_count : vhost_entry->readme_count;
							if(items != NULL) {
								int phase = 0;
							doit:
								for(i = 0; items[i] != NULL; i++) {
									int j;
									char* fpth = cm_strcat3(path, "/", items[i]);
									struct stat s;
									char size[512];
									size[0] = 0;
									stat(fpth, &s);
									if(phase == 0 && !S_ISDIR(s.st_mode)) {
										free(fpth);
										continue;
									} else if(phase == 1 && S_ISDIR(s.st_mode)) {
										free(fpth);
										continue;
									}
									if(readme == -1) {
										for(j = 0; j < readme_count; j++) {
											if(strcmp(items[i], readmes[j]) == 0) {
												readme = j;
												break;
											}
										}
										if(readme != -1) {
											free(fpth);
											continue;
										}
									}
									if(s.st_size < 1024ULL) {
										sprintf(size, "%d", (int)s.st_size);
									} else if(s.st_size < 1024ULL * 1024) {
										sprintf(size, "%.1fK", (double)s.st_size / 1024);
									} else if(s.st_size < 1024ULL * 1024 * 1024) {
										sprintf(size, "%.1fM", (double)s.st_size / 1024 / 1024);
									} else if(s.st_size < 1024ULL * 1024 * 1024 * 1024) {
										sprintf(size, "%.1fG", (double)s.st_size / 1024 / 1024 / 1024);
									} else if(s.st_size < 1024ULL * 1024 * 1024 * 1024 * 1024) {
										sprintf(size, "%.1fT", (double)s.st_size / 1024 / 1024 / 1024 / 1024);
									}

									free(fpth);

									char* ext = NULL;
									for(j = strlen(items[i]) - 1; j >= 0; j--) {
										if(items[i][j] == '.') {
											ext = cm_strdup(items[i] + j);
											break;
										} else if(items[i][j] == '/') {
											break;
										}
									}
									char* showmime = "";
									char* mime = tw_get_mime(ext, vhost_entry);
									if(strcmp(items[i], "../") == 0) {
										mime = "misc/parent";
										size[0] = 0;
									} else if(items[i][strlen(items[i]) - 1] == '/') {
										mime = "misc/dir";
										size[0] = 0;
									} else {
										showmime = mime;
									}
									char* icon = tw_get_icon(mime, vhost_entry);
									if(ext != NULL) free(ext);
									char* itm = cm_strdup(items[i]);
									if(strlen(itm) >= 32) {
										if(itm[strlen(itm) - 1] == '/') {
											itm[31] = 0;
											itm[30] = '/';
											itm[29] = '.';
											itm[28] = '.';
											itm[27] = '.';
										} else {
											itm[31] = 0;
											itm[30] = '.';
											itm[29] = '.';
											itm[28] = '.';
										}
									}
									addstring(&str, "<tr>\n");
									addstring(&str, "	<td><img src=\"%s\" alt=\"icon\"></td>\n", icon);
									addstring(&str, "	<td><a href=\"%l\"><code>%h</code></a></td>\n", items[i], itm);
									addstring(&str, "	<td><code>&emsp;&emsp;%h&emsp;&emsp;</code></td>\n", showmime);
									addstring(&str, "	<td><code>&emsp;&emsp;%s&emsp;&emsp;</code></td>\n", size);
									addstring(&str, "</tr>\n");
									free(itm);
								}
								phase++;
								if(phase != 2) goto doit;
								for(i = 0; items[i] != NULL; i++) free(items[i]);
								free(items);
							}
							addstring(&str, "		</table>\n");
							if(readme != -1) {
								addstring(&str, "<hr>\n");
								char* fpth = cm_strcat3(path, "/", readmes[readme]);
								struct stat s;
								stat(fpth, &s);
								FILE* fr = fopen(fpth, "r");
								if(fr != NULL) {
									char* rmbuf = malloc(s.st_size + 1);
									rmbuf[s.st_size] = 0;
									fread(rmbuf, s.st_size, 1, fr);
									addstring(&str, "<pre><code>%h</code></pre>\n", rmbuf);
									fclose(fr);
								}
							}
							addstring(&str, "		<hr>\n");
							addstring(&str, "		<address>%s Server at %s Port %d</address>\n", tw_server, name, port);
							addstring(&str, "	</body>\n");
							addstring(&str, "</html>\n");
							tw_process_page(s, sock, tw_http_status(200), "text/html", NULL, str, strlen(str), 0, 0);
							free(str);
						}
					}
				} else {
					char* ext = NULL;
					for(i = strlen(req.path) - 1; i >= 0; i--) {
						if(req.path[i] == '.') {
							ext = cm_strdup(req.path + i);
							break;
						} else if(req.path[i] == '/') {
							break;
						}
					}
					char* mime = tw_get_mime(ext, vhost_entry);
					if(ext != NULL) free(ext);
					FILE* f = fopen(path, "rb");
					tw_process_page(s, sock, tw_http_status(200), mime, f, NULL, st.st_size, st.st_mtime, cmtime);
					fclose(f);
				}
			} else {
				tw_http_error(s, sock, 404, name, port);
			}
			free(path);
		}
		free(vhost);
		free(host);
		tw_free_request(&req);
	} else if(ret == -1) {
	} else {
		tw_http_error(s, sock, 400, name, port);
	}
cleanup:
	if(sslworks) {
		SSL_shutdown(s);
	}
	SSL_free(s);
	close_socket(sock);
#ifdef __MINGW32__
	_endthreadex(0);
#endif
}

void tw_server_loop(void) {
	struct timeval tv;
	while(1) {
		FD_ZERO(&fdset);
		int i;
		for(i = 0; i < sockcount; i++) {
			FD_SET(sockets[i], &fdset);
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int ret = select(FD_SETSIZE, &fdset, NULL, NULL, &tv);
		if(ret == -1) {
			break;
		} else if(ret > 0) {
			/* connection */
			int i;
			for(i = 0; i < sockcount; i++) {
				if(FD_ISSET(sockets[i], &fdset)) {
					SOCKADDR claddr;
					int clen = sizeof(claddr);
					int sock = accept(sockets[i], (struct sockaddr*)&claddr, &clen);
					cm_log("Server", "New connection accepted");
#ifdef __MINGW32__
					HANDLE thread;
					struct pass_entry* e = malloc(sizeof(*e));
					e->sock = sock;
					e->ssl = config.ports[i] & (1ULL << 32);
					e->port = config.ports[i];
					e->addr = claddr;
					thread = (HANDLE)_beginthreadex(NULL, 0, tw_server_pass, e, 0, NULL);
#else
					pid_t pid = fork();
					if(pid == 0) {
						tw_server_pass(sock, config.ports[i] & (1ULL << 32), config.ports[i], claddr);
						_exit(0);
					} else {
						close_socket(sock);
					}
#endif
				}
			}
		}
	}
}
