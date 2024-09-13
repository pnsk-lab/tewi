/* $Id$ */

#include "tw_server.h"

#include "tw_ssl.h"
#include "tw_config.h"

#include <unistd.h>
#include <string.h>
#include <stdbool.h>

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

#ifdef __MINGW32__
struct pass_entry {
	int sock;
	int port;
	bool ssl;
};

unsigned int WINAPI tw_server_pass(void* ptr) {
	int sock = ((struct pass_entry*)ptr)->sock;
	bool ssl = ((struct pass_entry*)ptr)->ssl;
	int port = ((struct pass_entry*)ptR)->port;
	free(ptr);
#else
void tw_server_pass(int sock, bool ssl, int port) {
#endif
	SSL_CTX* ctx = NULL;
	SSL* s = NULL;
	if(ssl) {
		ctx = tw_create_ssl_ctx(port);
		s = SSL_new(ctx);
		SSL_set_fd(s, sock);
		if(SSL_accept(s) <= 0) goto cleanup;
	}
cleanup:
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
