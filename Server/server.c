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

#include <cm_string.h>
#include <cm_log.h>

#ifdef __MINGW32__
#include <winsock2.h>
#include <process.h>
#define NO_IPV6
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

#ifdef NO_IPV6
#define SOCKADDR struct sockaddr_in
#else
#define SOCKADDR struct sockaddr_in6
#endif
SOCKADDR addresses[MAX_PORTS];
int sockets[MAX_PORTS];

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

#define ERROR_400 \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" \
	"<html>\n" \
	"	<head>\n" \
	"		<title>400 Bad Request</title>" \
	"	</head>\n" \
	"	<body>\n" \
	"		<h1>Bad Request</h1>\n" \
	"		<hr>\n" \
	"		", \
	    address, \
	    "\n" \
	    "	</body>\n" \
	    "</html>\n"

#define ERROR_401 \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" \
	"<html>\n" \
	"	<head>\n" \
	"		<title>401 Unauthorized</title>" \
	"	</head>\n" \
	"	<body>\n" \
	"		<h1>Unauthorized</h1>\n" \
	"		<hr>\n" \
	"		", \
	    address, \
	    "\n" \
	    "	</body>\n" \
	    "</html>\n"

#define ERROR_403 \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" \
	"<html>\n" \
	"	<head>\n" \
	"		<title>403 Forbidden</title>" \
	"	</head>\n" \
	"	<body>\n" \
	"		<h1>Forbidden</h1>\n" \
	"		<hr>\n" \
	"		", \
	    address, \
	    "\n" \
	    "	</body>\n" \
	    "</html>\n"

#define ERROR_404 \
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" \
	"<html>\n" \
	"	<head>\n" \
	"		<title>404 Not Found</title>" \
	"	</head>\n" \
	"	<body>\n" \
	"		<h1>Not Found</h1>\n" \
	"		<hr>\n" \
	"		", \
	    address, \
	    "\n" \
	    "	</body>\n" \
	    "</html>\n"

void tw_process_page(SSL* ssl, int sock, const char* status, const char* type, const unsigned char* doc, size_t size) {
	char construct[512];
	sprintf(construct, "%llu", (unsigned long long)size);
	tw_write(ssl, sock, "HTTP/1.1 ", 9);
	tw_write(ssl, sock, (char*)status, strlen(status));
	tw_write(ssl, sock, "\r\n", 2);
	tw_write(ssl, sock, "Content-Type: ", 7 + 5 + 2);
	tw_write(ssl, sock, (char*)type, strlen(type));
	tw_write(ssl, sock, "\r\n", 2);
	tw_write(ssl, sock, "Server: ", 6 + 2);
	tw_write(ssl, sock, tw_server, strlen(tw_server));
	tw_write(ssl, sock, "\r\n", 2);
	tw_write(ssl, sock, "Content-Length: ", 7 + 7 + 2);
	tw_write(ssl, sock, construct, strlen(construct));
	tw_write(ssl, sock, "\r\n", 2);
	tw_write(ssl, sock, "\r\n", 2);
	size_t incr = 0;
	while(1) {
		tw_write(ssl, sock, (unsigned char*)doc + incr, size < 128 ? size : 128);
		incr += 128;
		if(size <= 128) break;
		size -= 128;
	}
}

const char* tw_http_status(int code) {
	if(code == 400) {
		return "400 Bad Request";
	} else {
		return "400 Bad Request";
	}
}

char* tw_http_default_error(int code, char* name, int port) {
	char address[1024];
	sprintf(address, "<address>%s Server at %s Port %d</address>", tw_server, name, port);
	if(code == 400) {
		return cm_strcat3(ERROR_400);
	} else {
		return cm_strcat3(ERROR_400);
	}
}

void tw_http_error(SSL* ssl, int sock, int error, char* name, int port) {
	char* str = tw_http_default_error(error, name, port);
	tw_process_page(ssl, sock, tw_http_status(error), "text/html", str, strlen(str));
	free(str);
}

#ifdef __MINGW32__
struct pass_entry {
	int sock;
	int port;
	bool ssl;
};

unsigned int WINAPI tw_server_pass(void* ptr) {
	int sock = ((struct pass_entry*)ptr)->sock;
	bool ssl = ((struct pass_entry*)ptr)->ssl;
	int port = ((struct pass_entry*)ptr)->port;
	free(ptr);
#else
void tw_server_pass(int sock, bool ssl, int port) {
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
	int ret = tw_http_parse(s, sock, &req);
	if(ret == 0) {
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
					thread = (HANDLE)_beginthreadex(NULL, 0, tw_server_pass, e, 0, NULL);
#else
					pid_t pid = fork();
					if(pid == 0) {
						tw_server_pass(sock, config.ports[i] & (1ULL << 32), config.ports[i]);
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
