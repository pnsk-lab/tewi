/* $Id$ */

#define SOURCE

#include "../config.h"

#include "tw_server.h"

#ifndef NO_SSL
#include "tw_ssl.h"
#endif

#include "tw_config.h"
#include "tw_http.h"
#include "tw_module.h"
#include "tw_version.h"

#ifdef __amiga__
#include <pthread.h>
#endif

#if !defined(_MSC_VER) && !defined(__BORLANDC__)
#include <unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <cm_string.h>
#include <cm_log.h>
#include <cm_dir.h>

#ifdef __OS2__
#include <types.h>
#include <sys/time.h>
#define INCL_DOSPROCESS
#include <os2.h>
#include <process.h>
#define HANDLE void*

#include "strptime.h"
typedef int socklen_t;

#include <tcpustd.h>
#endif

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || (defined(__WATCOMC__) && !defined(__OS2__) && !defined(__NETWARE__) && !defined(__DOS__))
#ifndef NO_GETNAMEINFO
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#ifdef USE_WINSOCK1
#include <winsock.h>
#else
#include <winsock2.h>
#endif
#include <process.h>
#include <windows.h>

#include "strptime.h"
typedef int socklen_t;
#elif defined(__NETWARE__)
#include <sys/bsdskt.h>
#include <sys/socket.h>

#define IPPROTO_TCP 0
#define INADDR_ANY 0
#include "strptime.h"
typedef int socklen_t;

uint16_t htons(uint16_t n) { return ((n >> 8) & 0xff) | ((n << 8) & 0xff00); }
#elif defined(__DOS__)
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "strptime.h"
#else
#ifdef USE_POLL
#ifdef __PPU__
#include <net/poll.h>
#else
#include <poll.h>
#endif
#else
#ifndef __NeXT__
#include <sys/select.h>
#endif
#ifdef __OS2__
#include <netinet/in.h>
#endif
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#if !defined(__PPU__)
#ifdef __NeXT__
#include <netinet/in_systm.h>
#endif
#include <netinet/tcp.h>
#endif
#ifndef NO_GETNAMEINFO
#include <netdb.h>
#endif
#endif

#if defined(_PSP) || defined(__ps2sdk__) || defined(__bsdi__) || defined(__amiga__)
#include "strptime.h"
#endif

#ifdef __HAIKU__
#include <OS.h>
#endif

#ifdef __NeXT__
#include <sys/time.h>
#endif

#if defined(__USLC__) || defined(__NeXT__)
typedef int socklen_t;
#endif

#ifndef S_ISDIR
#define S_ISDIR(x) ((x) & _S_IFDIR)
#endif

extern struct tw_config config;
extern char tw_server[];

int sockcount = 0;

SOCKADDR addresses[MAX_PORTS];
int sockets[MAX_PORTS];

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
const char* reserved_names[] = {"CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
#endif

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
		} else if(*pw == '?' || (tolower(*pw) == tolower(*pt))) {
			pw++;
			pt++;
			continue;
		} else {
			return 0;
		}
	}
}

void close_socket(int sock) {
#ifdef __OS2__
	soclose(sock);
#elif defined(__NETWARE__)
	shutdown(sock, 2);
#elif defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
	closesocket(sock);
#else
	close(sock);
#endif
}

int tw_server_init(void) {
	int i;
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || (defined(__WATCOMC__) && !defined(__OS2__) && !defined(__NETWARE__) && !defined(__DOS__))
	WSADATA wsa;
#ifdef USE_WINSOCK1
	WSAStartup(MAKEWORD(1, 1), &wsa);
#else
	WSAStartup(MAKEWORD(2, 0), &wsa);
#endif
#endif
#ifdef __OS2__
	sock_init();
#endif
	for(i = 0; config.ports[i] != -1; i++)
		;
	sockcount = i;
	for(i = 0; config.ports[i] != -1; i++) {
		int yes = 1;
		int no = 0;
#ifdef NO_IPV6
		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
		int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
#endif
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__)
		if(sock == INVALID_SOCKET)
#else
		if(sock < 0)
#endif
		{
			cm_log("Server", "Socket creation failure");
			return 1;
		}
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0) {
			close_socket(sock);
			cm_log("Server", "setsockopt failure (reuseaddr)");
			return 1;
		}
#if !defined(__PPU__) && !defined(__minix)
		if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&yes, sizeof(yes)) < 0) {
			close_socket(sock);
			cm_log("Server", "setsockopt failure (nodelay)");
			return 1;
		}
#endif
#ifndef NO_IPV6
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
		addresses[i].sin_port = htons(config.ports[i] & 0xffff);
#else
		addresses[i].sin6_family = AF_INET6;
		addresses[i].sin6_addr = in6addr_any;
		addresses[i].sin6_port = htons(config.ports[i] & 0xffff);
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
#ifndef NO_SSL
	if(ssl == NULL) {
		return recv(s, data, len, 0);
	} else {
		return SSL_read(ssl, data, len);
	}
#else
	return recv(s, data, len, 0);
#endif
}

size_t tw_write(SSL* ssl, int s, void* data, size_t len) {
#ifndef NO_SSL
	if(ssl == NULL) {
		return send(s, data, len, 0);
	} else {
		return SSL_write(ssl, data, len);
	}
#else
	return send(s, data, len, 0);
#endif
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
	size_t incr;
	if(mtime != 0 && cmtime != 0 && mtime <= cmtime) {
		status = "304 Not Modified";
		type = NULL;
		size = 0;
		headers = NULL;
		f = NULL;
		doc = NULL;
	}
#if defined(_MSC_VER) || defined(__BORLANDC__)
	sprintf(construct, "%lu", (unsigned long)size);
#else
	sprintf(construct, "%llu", (unsigned long long)size);
#endif
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
	if(headers != NULL) {
		int i;
		for(i = 0; headers[i] != NULL; i += 2) {
			tw_write(ssl, sock, headers[i], strlen(headers[i]));
			tw_write(ssl, sock, ": ", 2);
			tw_write(ssl, sock, headers[i + 1], strlen(headers[i + 1]));
			tw_write(ssl, sock, "\r\n", 2);
		}
	}
	tw_write(ssl, sock, "\r\n", 2);
	if(doc == NULL && f == NULL) return;
	incr = 0;
	while(1) {
		if(f != NULL) {
			char buffer[512];
			int st;
			fread(buffer, size < 512 ? size : 512, 1, f);
			if((st = tw_write(ssl, sock, buffer, size < 512 ? size : 512)) <= 0) break;
		} else {
			if(tw_write(ssl, sock, (unsigned char*)doc + incr, size < 512 ? size : 512) <= 0) break;
		}
		incr += 512;
		if(size <= 512) break;
		size -= 512;
	}
}

void tw_process_page(SSL* ssl, int sock, const char* status, const char* type, FILE* f, const unsigned char* doc, size_t size, time_t mtime, time_t cmtime) { _tw_process_page(ssl, sock, status, type, f, doc, size, NULL, mtime, cmtime); }

const char* tw_http_status(int code) {
	if(code == 200) {
		return "200 OK";
	} else if(code == 301) {
		return "301 Moved Permanently";
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
	} else if(code == 500) {
		return "500 Internal Server Error";
	} else {
		return "400 Bad Request";
	}
}

char* tw_http_default_error(int code, char* name, int port, struct tw_config_entry* vhost) {
	char address[1024];
	char* st;
	char* st2;
	char* buffer;
	char* str;
	int i;

	if((vhost->hideport == -1 ? config.root.hideport : vhost->hideport) == 1) {
		sprintf(address, "<address>%s Server at %s</address>", tw_server, name, port);
	} else {
		sprintf(address, "<address>%s Server at %s Port %d</address>", tw_server, name, port);
	}

	st = cm_strdup(tw_http_status(code));
	for(i = 0; st[i] != 0; i++) {
		if(st[i] == ' ') {
			st2 = cm_strdup(st + i + 1);
			break;
		}
	}
	buffer = malloc(4096);
	str = cm_strcat3(ERROR_HTML);
	sprintf(buffer, str, st, st2);
	free(str);
	free(st);
	return buffer;
}

void tw_http_error(SSL* ssl, int sock, int error, char* name, int port, struct tw_config_entry* vhost) {
	char* str = tw_http_default_error(error, name, port, vhost);
	tw_process_page(ssl, sock, tw_http_status(error), "text/html", NULL, str, strlen(str), 0, 0);
	free(str);
}

void addstring(char** str, const char* add, ...) {
	int i;
	char cbuf[2];
	va_list va;
	cbuf[1] = 0;
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
				char* tmp = *str;
				sprintf(h, "%d", n);
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
	va_end(va);
}

char* tw_get_mime(const char* ext, struct tw_config_entry* vhost_entry) {
	char* mime = "application/octet-stream";
	bool set = false;
	int i;
	if(ext == NULL) return mime;
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
	bool set = false;
	int i;
	if(mime == NULL) return "";
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

struct pass_entry {
	int sock;
	int port;
	bool ssl;
	SOCKADDR addr;
};

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#define NO_RETURN_THREAD
void tw_server_pass(void* ptr) {
#elif defined(__amiga__)
void* tw_server_pass(void* ptr) {
#elif defined(__HAIKU__)
int32_t tw_server_pass(void* ptr) {
#elif defined(_PSP) || defined(__PPU__)
int tw_server_pass(void* ptr) {
#endif
#if defined(__HAIKU__) || defined(__MINGW32__) || defined(_PSP) || defined(__PPU__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__amiga__)
#define FREE_PTR
	int sock = ((struct pass_entry*)ptr)->sock;
	bool ssl = ((struct pass_entry*)ptr)->ssl;
	int port = ((struct pass_entry*)ptr)->port;
	SOCKADDR addr = ((struct pass_entry*)ptr)->addr;
#else
#define NO_RETURN_THREAD
	void tw_server_pass(int sock, bool ssl, int port, SOCKADDR addr) {
#endif
	SSL* s = NULL;
#ifndef NO_SSL
	SSL_CTX* ctx = NULL;
	bool sslworks = false;
	if(ssl) {
		ctx = tw_create_ssl_ctx(port);
		s = SSL_new(ctx);
		SSL_set_fd(s, sock);
		if(SSL_accept(s) <= 0) goto cleanup;
		sslworks = true;
	}
#endif
	char* name = config.hostname;
	char address[513];
	int ret;
	struct tw_http_request req;
	struct tw_http_response res;
	struct tw_tool tools;
	char* addrstr;
#ifndef NO_GETNAMEINFO
	struct sockaddr* sa = (struct sockaddr*)&addr;
	getnameinfo(sa, sizeof(addr), address, 512, NULL, 0, NI_NUMERICHOST);
#elif defined(__NETWARE__)
		address[0] = 0;
#else
	addrstr = inet_ntoa(addr.sin_addr);
	strcpy(address, addrstr);
	address[strlen(addrstr)] = 0;
#endif
#ifdef FREE_PTR
	free(ptr);
#endif

	res._processed = false;
	tw_init_tools(&tools);
	ret = tw_http_parse(s, sock, &req);
	if(ret == 0) {
		char date[513];
		time_t t = time(NULL);
		struct tm* tm = localtime(&t);
		char* useragent = cm_strdup("");
		int i;
		char* tmp;
		char* tmp2;
		char* tmp3;
		char* tmp4;
		char* tmp5;
		char* log;
		char* vhost;
		time_t cmtime;
		bool rej;
		char* host;
		int port;
		char* chrootpath;
		struct tw_config_entry* vhost_entry;
		strftime(date, 512, "%a, %d %b %Y %H:%M:%S %Z", tm);

		for(i = 0; req.headers[i] != NULL; i += 2) {
			if(cm_strcaseequ(req.headers[i], "User-Agent")) {
				free(useragent);
				useragent = cm_strdup(req.headers[i + 1]);
			}
		}

		tmp = cm_strcat3(address, " - [", date);
		tmp2 = cm_strcat3(tmp, "] \"", req.method);
		tmp3 = cm_strcat3(tmp2, " ", req.path);
		tmp4 = cm_strcat3(tmp3, " ", req.version);
		tmp5 = cm_strcat3(tmp4, "\" \"", useragent);
		log = cm_strcat(tmp5, "\"");
		free(tmp);
		free(tmp2);
		free(tmp3);
		free(tmp4);
		free(tmp5);
		free(useragent);
		cm_force_log(log);
		free(log);

		vhost = cm_strdup(config.hostname);
		cmtime = 0;
		if(req.headers != NULL) {
			for(i = 0; req.headers[i] != NULL; i += 2) {
				if(cm_strcaseequ(req.headers[i], "Host")) {
					free(vhost);
					vhost = cm_strdup(req.headers[i + 1]);
				} else if(cm_strcaseequ(req.headers[i], "If-Modified-Since")) {
					struct tm tm;
					time_t t;
					struct tm* btm;
					strptime(req.headers[i + 1], "%a, %d %b %Y %H:%M:%S GMT", &tm);
#if defined(__MINGW32__) || defined(_PSP) || defined(__PPU__) || defined(__ps2sdk__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__USLC__) || defined(__NeXT__) || defined(__bsdi__) || defined(__amiga__) || defined(__sun__)
					t = 0;
					btm = localtime(&t);
					cmtime = mktime(&tm);
					cmtime -= (btm->tm_hour * 60 + btm->tm_min) * 60;
#else
						cmtime = timegm(&tm);
#endif
				}
			}
		}
		rej = false;
		cm_log("Server", "Host is %s", vhost);
		port = s == NULL ? 80 : 443;
		host = cm_strdup(vhost);
		for(i = 0; vhost[i] != 0; i++) {
			if(vhost[i] == ':') {
				host[i] = 0;
				port = atoi(host + i + 1);
				break;
			}else if(vhost[i] == '['){
				for(; vhost[i] != 0 && vhost[i] != ']'; i++);
			}
		}
		name = host;
		cm_log("Server", "Hostname is `%s', port is `%d'", host, port);
		vhost_entry = tw_vhost_match(host, port);
#ifdef HAS_CHROOT
		chrootpath = vhost_entry->chroot_path != NULL ? vhost_entry->chroot_path : config.root.chroot_path;
		if(chrootpath != NULL) {
			if(chdir(chrootpath) == 0) {
				if(chroot(".") == 0) {
					cm_log("Server", "Chroot successful");
				}
			} else {
				cm_log("Server", "chdir() failed, cannot chroot");
				tw_http_error(s, sock, 500, name, port, vhost_entry);
				rej = true;
			}
		}
#endif
		for(i = 0; i < config.module_count; i++) {
#ifdef __OS2__
			tw_mod_request_t mod_req = (tw_mod_request_t)tw_module_symbol(config.modules[i], "MOD_REQUEST");
#else
				tw_mod_request_t mod_req = (tw_mod_request_t)tw_module_symbol(config.modules[i], "mod_request");
#endif
			if(mod_req != NULL) {
				int ret = mod_req(&tools, &req, &res);
				int co = ret & 0xff;
				if(co == _TW_MODULE_PASS) {
					continue;
				} else if(co == _TW_MODULE_STOP) {
					/* Handle response here ... */
					res._processed = true;
					break;
				} else if(co == _TW_MODULE_STOP2) {
					res._processed = true;
					break;
				} else if(co == _TW_MODULE_ERROR) {
					tw_http_error(s, sock, (ret & 0xffff00) >> 8, name, port, vhost_entry);
					break;
				}
			}
		}
		if(!res._processed) {
			char* path;
			char* rpath;
			struct stat st;
			char* slash;
			cm_log("Server", "Document root is %s", vhost_entry->root == NULL ? "not set" : vhost_entry->root);
			path = cm_strcat(vhost_entry->root == NULL ? "" : vhost_entry->root, req.path);
			cm_log("Server", "Filesystem path is %s", path);
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
			for(i = strlen(path) - 1; i >= 0; i--) {
				if(path[i] == '/') {
					path[i] = 0;
				} else {
					break;
				}
			}
#endif
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
			rpath = cm_strdup(path);
			for(i = strlen(rpath) - 1; i >= 0; i--) {
				if(rpath[i] == '/') {
					int j;
					for(j = i + 1; rpath[j] != 0; j++) {
						if(rpath[j] == ':' || rpath[j] == '.') {
							rpath[j] = 0;
							break;
						}
					}
					break;
				}
			}
			for(i = 0; i < sizeof(reserved_names) / sizeof(reserved_names[0]); i++) {
				char* n = cm_strcat("/", reserved_names[i]);
				if(cm_nocase_endswith(rpath, n)) {
					tw_http_error(s, sock, 403, name, port, vhost_entry);
					free(n);
					rej = true;
					cm_log("Server", "XP Patch ; rejecting access to device");
					break;
				}
				free(n);
			}
			free(rpath);
#endif
			if(!rej && stat(path, &st) == 0) {
				if(!tw_permission_allowed(path, addr, req, vhost_entry)) {
					tw_http_error(s, sock, 403, name, port, vhost_entry);
				} else if(S_ISDIR(st.st_mode)) {
					if(req.path[strlen(req.path) - 1] != '/') {
						char* headers[3] = {"Location", NULL, NULL};
						headers[1] = cm_strcat(req.path, "/");
						cm_log("Server", "Accessing directory without the slash at the end");
						_tw_process_page(s, sock, tw_http_status(301), NULL, NULL, NULL, 0, headers, 0, 0);
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
								struct stat st;
								char* mime;
								for(j = strlen(p) - 1; j >= 0; j--) {
									if(p[j] == '.') {
										ext = cm_strdup(p + j);
										break;
									} else if(p[j] == '/') {
										break;
									}
								}
								stat(p, &st);
								mime = tw_get_mime(ext, vhost_entry);
								tw_process_page(s, sock, tw_http_status(200), mime, f, NULL, st.st_size, 0, 0);
								fclose(f);
								if(ext != NULL) free(ext);
								free(p);
								found = true;
								break;
							}
							free(p);
						}
						if(!found) {
							char* str = malloc(1);
							char** items;
							int readme;
							char** readmes;
							int readme_count;
							int hp;
							str[0] = 0;
							items = cm_scandir(path);
							addstring(&str, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n");
							addstring(&str, "<html>\n");
							addstring(&str, "	<head>\n");
							addstring(&str, "		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
							addstring(&str, "		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
							addstring(&str, "		<title>Index of %h</title>\n", req.path);
							addstring(&str, "	</head>\n");
							addstring(&str, "	<body>\n");
							addstring(&str, "		<h1>Index of %h</h1>\n", req.path);
							addstring(&str, "		<hr>\n");
							addstring(&str, "		<table border=\"0\">\n");
							addstring(&str, "			<tr>\n");
							addstring(&str, "				<th></th>\n");
							addstring(&str, "				<th>Filename</th>\n");
							addstring(&str, "				<th>Last-modified</th>\n");
							addstring(&str, "				<th>MIME</th>\n");
							addstring(&str, "				<th>Size</th>\n");
							addstring(&str, "			</tr>\n");
							readme = -1;
							readmes = vhost_entry->readme_count == 0 ? config.root.readmes : vhost_entry->readmes;
							readme_count = vhost_entry->readme_count == 0 ? config.root.readme_count : vhost_entry->readme_count;
							if(items != NULL) {
								int phase = 0;
							doit:
								for(i = 0; items[i] != NULL; i++) {
									int j;
									char* ext;
									char* itm;
									char* icon;
									char* fpth = cm_strcat3(path, "/", items[i]);
									struct stat s;
									char size[512];
									char date[512];
									char* showmime;
									char* mime;
									struct tm* tm;
									size[0] = 0;
									stat(fpth, &s);
									tm = localtime(&s.st_mtime);
									strftime(date, 512, "%a, %d %b %Y %H:%M:%S %Z", tm);
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
									if(s.st_size < NUM1024) {
										sprintf(size, "%d", (int)s.st_size);
									} else if(s.st_size < NUM1024 * 1024) {
										sprintf(size, "%.1fK", (double)s.st_size / 1024);
									} else if(s.st_size < NUM1024 * 1024 * 1024) {
										sprintf(size, "%.1fM", (double)s.st_size / 1024 / 1024);
									} else if(s.st_size < NUM1024 * 1024 * 1024 * 1024) {
										sprintf(size, "%.1fG", (double)s.st_size / 1024 / 1024 / 1024);
									} else if(s.st_size < NUM1024 * 1024 * 1024 * 1024 * 1024) {
										sprintf(size, "%.1fT", (double)s.st_size / 1024 / 1024 / 1024 / 1024);
									}

									free(fpth);

									ext = NULL;
									for(j = strlen(items[i]) - 1; j >= 0; j--) {
										if(items[i][j] == '.') {
											ext = cm_strdup(items[i] + j);
											break;
										} else if(items[i][j] == '/') {
											break;
										}
									}
									showmime = "";
									mime = tw_get_mime(ext, vhost_entry);
									if(strcmp(items[i], "../") == 0) {
										mime = "misc/parent";
										size[0] = 0;
									} else if(items[i][strlen(items[i]) - 1] == '/') {
										mime = "misc/dir";
										size[0] = 0;
									} else {
										showmime = mime;
									}
									icon = tw_get_icon(mime, vhost_entry);
									if(ext != NULL) free(ext);
									itm = cm_strdup(items[i]);
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
									addstring(&str, "	<td><code>  %h  </code></td>\n", date);
									addstring(&str, "	<td><code>  %h  </code></td>\n", showmime);
									addstring(&str, "	<td><code>  %s  </code></td>\n", size);
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
								struct stat s;
								FILE* fr;
								char* fpth;
								addstring(&str, "<hr>\n");
								fpth = cm_strcat3(path, "/", readmes[readme]);
								stat(fpth, &s);
								fr = fopen(fpth, "r");
								if(fr != NULL) {
									char* rmbuf = malloc(s.st_size + 1);
									rmbuf[s.st_size] = 0;
									fread(rmbuf, s.st_size, 1, fr);
									addstring(&str, "<pre><code>%h</code></pre>\n", rmbuf);
									fclose(fr);
									free(rmbuf);
								}
								free(fpth);
							}
							addstring(&str, "		<hr>\n");
							hp = vhost_entry->hideport == -1 ? config.root.hideport : vhost_entry->hideport;
							if(hp == 0) {
								addstring(&str, "		<address>%s Server at %s Port %d</address>\n", tw_server, name, port);
							} else {
								addstring(&str, "		<address>%s Server at %s</address>\n", tw_server, name, port);
							}
							addstring(&str, "	</body>\n");
							addstring(&str, "</html>\n");
							tw_process_page(s, sock, tw_http_status(200), "text/html", NULL, str, strlen(str), 0, 0);
							free(str);
						}
					}
				} else {
					char* ext = NULL;
					char* mime;
					FILE* f;
					for(i = strlen(req.path) - 1; i >= 0; i--) {
						if(req.path[i] == '.') {
							ext = cm_strdup(req.path + i);
							break;
						} else if(req.path[i] == '/') {
							break;
						}
					}
					mime = tw_get_mime(ext, vhost_entry);
					if(ext != NULL) free(ext);
					f = fopen(path, "rb");
					if(f == NULL) {
						tw_http_error(s, sock, 403, name, port, vhost_entry);
					} else {
						tw_process_page(s, sock, tw_http_status(200), mime, f, NULL, st.st_size, st.st_mtime, cmtime);
						fclose(f);
					}
				}
			} else {
				if(!rej) {
					tw_http_error(s, sock, 404, name, port, vhost_entry);
				}
			}
			free(path);
		}
		free(vhost);
		free(host);
	} else if(ret == -1) {
	} else {
		tw_http_error(s, sock, 400, name, port, &config.root);
	}
	tw_free_request(&req);
cleanup:
#ifndef NO_SSL
	if(sslworks) {
		SSL_shutdown(s);
	}
	SSL_free(s);
#endif
	close_socket(sock);
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#ifdef __NETWARE__
	ExitThread(EXIT_THREAD, 0);
#elif defined(__DOS__)
#else
	_endthread();
#endif
#elif defined(__HAIKU__)
		exit_thread(0);
#endif
#ifndef NO_RETURN_THREAD
	return 0;
#endif
}

#ifdef SERVICE
extern SERVICE_STATUS status;
extern SERVICE_STATUS_HANDLE status_handle;
#endif

#if defined(__MINGW32__) || defined(__HAIKU__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
struct thread_entry {
#ifdef __HAIKU__
	thread_id thread;
#elif defined(__NETWARE__)
	int thread;
#elif defined(__DOS__)
#else
	HANDLE handle;
#endif
	bool used;
};
#endif

extern int running;

void tw_server_loop(void) {
	int i;
#ifndef USE_POLL
	fd_set fdset;
	struct timeval tv;
#endif
#if defined(__MINGW32__) || defined(__HAIKU__) || defined(_MSC_VER) || defined(__BORLANDC__) || (defined(__WATCOMC__) && !defined(__NETWARE__) && !defined(__DOS__))
	struct thread_entry threads[128];
	for(i = 0; i < sizeof(threads) / sizeof(threads[0]); i++) {
		threads[i].used = false;
	}
#endif
#ifdef USE_POLL
	struct pollfd* pollfds = malloc(sizeof(*pollfds) * sockcount);
	for(i = 0; i < sockcount; i++) {
		pollfds[i].fd = sockets[i];
		pollfds[i].events = POLLIN | POLLPRI;
	}
#endif
	while(running) {
		int ret;
#ifdef USE_POLL
		ret = poll(pollfds, sockcount, 1000);
#else
			FD_ZERO(&fdset);
			for(i = 0; i < sockcount; i++) {
				FD_SET(sockets[i], &fdset);
			}
			tv.tv_sec = 1;
			tv.tv_usec = 0;
#ifdef __HAIKU__
			ret = select(32, &fdset, NULL, NULL, &tv);
#else
			ret = select(FD_SETSIZE, &fdset, NULL, NULL, &tv);
#endif
#endif
		if(ret == -1) {
#if !defined(__MINGW32__) && !defined(_MSC_VER) && !defined(__BORLANDC__) && !defined(__WATCOMC__)
			if(errno == EINTR) continue;
			cm_log("Server", "Select/poll failure: %s", strerror(errno));
#endif
			break;
		} else if(ret == 0) {
#ifdef SERVICE
			if(status.dwCurrentState == SERVICE_STOP_PENDING) {
				break;
			}
#endif
		} else if(ret > 0) {
			/* connection */
			int i;
			for(i = 0; i < sockcount; i++) {
				bool cond;
#ifdef USE_POLL
				cond = pollfds[i].revents & POLLIN;
#else
					cond = FD_ISSET(sockets[i], &fdset);
#endif
				if(cond) {
					SOCKADDR claddr;
					socklen_t clen = sizeof(claddr);
					int sock = accept(sockets[i], (struct sockaddr*)&claddr, &clen);
#if defined(__MINGW32__) || defined(__HAIKU__) || defined(_PSP) || defined(__PPU__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__amiga__)
#ifdef __amiga__
					pthread_t thrt;
#endif
					int j;
					struct pass_entry* e = malloc(sizeof(*e));
					cm_log("Server", "New connection accepted");
					e->sock = sock;
#if defined(_MSC_VER) || defined(__BORLANDC__)
					e->ssl = config.ports[i] & (1UL << 31);
#else
					e->ssl = config.ports[i] & (1ULL << 31);
#endif
					e->port = config.ports[i];
					e->addr = claddr;
#endif
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#ifdef __OS2__
					_beginthread(tw_server_pass, 0, 0, e);
#elif defined(__NETWARE__)
					BeginThread(tw_server_pass, NULL, 0, e);
#elif defined(__DOS__)
					tw_server_pass(e);
#else
					_beginthread(tw_server_pass, 0, e);
#endif
#elif defined(_PSP) || defined(__PPU__)
						tw_server_pass(e);
#elif defined(__amiga__)
					pthread_create(&thrt, NULL, tw_server_pass, e);
#elif defined(__HAIKU__)
					for(j = 0; j < sizeof(threads) / sizeof(threads[0]); j++) {
						if(threads[j].used) {
							thread_info info;
							bool kill = false;
							if(get_thread_info(threads[j].thread, &info) == B_OK) {
							} else {
								kill = true;
							}
							if(kill) {
								threads[j].used = false;
							}
						}
					}
					for(j = 0; j < sizeof(threads) / sizeof(threads[0]); j++) {
						if(!threads[j].used) {
							threads[j].thread = spawn_thread(tw_server_pass, "Tewi HTTPd", 60, e);
							threads[j].used = true;
							resume_thread(threads[j].thread);
							break;
						}
					}
#else
					pid_t pid = fork();
					if(pid == 0) {
						int j;
						for(j = 0; j < sockcount; j++) close_socket(sockets[j]);
						tw_server_pass(sock, config.ports[i] & (1ULL << 31), config.ports[i], claddr);
						_exit(0);
					} else {
						close_socket(sock);
					}
#endif
				}
			}
		}
	}
	for(i = 0; i < sockcount; i++) {
		close_socket(sockets[i]);
	}
	cm_force_log("Server is down");
}
