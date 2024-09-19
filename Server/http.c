/* $Id$ */

#define SOURCE

#include "../config.h"

#include "tw_http.h"

#include "tw_server.h"

#include <cm_log.h>
#include <cm_string.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#include <winsock2.h>
#else
#include <sys/select.h>
#endif

void tw_free_request(struct tw_http_request* req) {
	if(req->method != NULL) free(req->method);
	if(req->path != NULL) free(req->path);
	if(req->query != NULL) free(req->query);
	if(req->headers != NULL) {
		int i;
		for(i = 0; req->headers[i] != NULL; i++) free(req->headers[i]);
		free(req->headers);
	}
	if(req->body != NULL) free(req->body);
	if(req->version != NULL) free(req->version);

	req->method = NULL;
	req->path = NULL;
	req->query = NULL;
	req->headers = NULL;
	req->body = NULL;
	req->version = NULL;
}

int tw_http_parse(SSL* ssl, int sock, struct tw_http_request* req) {
	char buffer[512];
	char cbuf[2];
	int phase = 0;
	fd_set fds;

	bool bad = false;

	cbuf[1] = 0;

	req->method = NULL;
	req->path = NULL;
	req->query = NULL;
	req->headers = NULL;
	req->body = NULL;
	req->version = NULL;

	char* header = malloc(1);
	header[0] = 0;
	int nl = 0;

	while(1) {
		FD_ZERO(&fds);
		FD_SET(sock, &fds);
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
#ifndef NO_SSL
		if(ssl == NULL || !SSL_has_pending(ssl)) {
#endif
#ifdef __HAIKU__
			int n = select(32, &fds, NULL, NULL, &tv);
#else
			int n = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
#endif
			if(n <= 0) {
				cm_log("HTTP", "Timeout, disconncting");
				free(header);
				tw_free_request(req);
				return -1;
			}
#ifndef NO_SSL
		}
#endif
		int len = tw_read(ssl, sock, buffer, 512);
		if(len <= 0) {
			bad = true;
			break;
		}
		int i;
		for(i = 0; i < len; i++) {
			char c = buffer[i];
			if(phase == 0) {
				if(c == ' ') {
					if(req->method == NULL) {
						tw_free_request(req);
						bad = true;
						goto getout;
					} else {
						phase++;
					}
				} else {
					if(req->method == NULL) {
						req->method = malloc(1);
						req->method[0] = 0;
					}
					cbuf[0] = c;
					char* tmp = req->method;
					req->method = cm_strcat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 1) {
				if(c == ' ') {
					if(req->path == NULL) {
						tw_free_request(req);
						bad = true;
						goto getout;
					} else {
						phase++;
					}
				} else {
					if(req->path == NULL) {
						req->path = malloc(1);
						req->path[0] = 0;
					}
					cbuf[0] = c;
					char* tmp = req->path;
					req->path = cm_strcat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 2) {
				if(c == '\n') {
					if(req->version == NULL) {
						tw_free_request(req);
						bad = true;
						goto getout;
					} else {
						/* We have Method, Path, Version now */

						if(strcmp(req->version, "HTTP/1.1") != 0 && strcmp(req->version, "HTTP/1.0") != 0) {
							cm_log("HTTP", "Bad HTTP Version");
							bad = true;
							goto getout;
						}

						int j;
						char* p = malloc(1);
						p[0] = 0;
						for(j = 0; req->path[j] != 0; j++) {
							if(req->path[j] == '/') {
								cbuf[0] = '/';
								for(; req->path[j] != 0 && req->path[j] == '/'; j++)
									;
								j--;
							} else {
								cbuf[0] = req->path[j];
							}
							char* tmp = p;
							p = cm_strcat(tmp, cbuf);
							free(tmp);
						}
						free(req->path);
						req->path = p;

						int incr = 0;
						p = malloc(1);
						p[0] = 0;
						for(j = 0;; j++) {
							if(req->path[j] == '/' || req->path[j] == 0) {
								char oldc = req->path[j];
								cbuf[0] = oldc;
								req->path[j] = 0;

								char* pth = req->path + incr;

								if(strcmp(pth, "..") == 0) {
									int k;
									if(p[strlen(p) - 1] == '/') p[strlen(p) - 1] = 0;
									for(k = strlen(p) - 1; k >= 0; k--) {
										if(p[k] == '/') {
											p[k + 1] = 0;
											break;
										}
									}
									if(strlen(p) == 0) {
										free(p);
										p = cm_strdup("/");
									}
								} else if(strcmp(pth, ".") == 0) {
								} else {
									char* tmp = p;
									p = cm_strcat3(tmp, pth, cbuf);
									free(tmp);
								}

								incr = j + 1;
								if(oldc == 0) break;
							}
						}
						free(req->path);
						req->path = p;

						cm_log("HTTP", "Request: %s %s %s", req->method, req->path, req->version);

						phase++;
					}
				} else if(c != '\r') {
					if(req->version == NULL) {
						req->version = malloc(1);
						req->version[0] = 0;
					}
					cbuf[0] = c;
					char* tmp = req->version;
					req->version = cm_strcat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 3) {
				if(c == '\n') {
					nl++;
					if(nl == 2) {
						phase++;
						goto getout;
					} else {
						if(req->headers == NULL) {
							req->headers = malloc(sizeof(*req->headers));
							req->headers[0] = NULL;
						}
						int j;
						for(j = 0; header[j] != 0; j++) {
							if(header[j] == ':') {
								header[j] = 0;
								j++;
								for(; header[j] != 0 && (header[j] == ' ' || header[j] == '\t'); j++)
									;
								char* kv = header;
								char* vv = header + j;

								char** old = req->headers;
								int k;
								for(k = 0; old[k] != NULL; k++)
									;
								req->headers = malloc(sizeof(*req->headers) * (k + 3));
								for(k = 0; old[k] != NULL; k++) req->headers[k] = old[k];
								req->headers[k] = cm_strdup(kv);
								req->headers[k + 1] = cm_strdup(vv);
								req->headers[k + 2] = NULL;
								free(old);

								cm_log("HTTP", "Header: %s: %s", kv, vv);

								break;
							}
						}
						free(header);
						header = malloc(1);
						header[0] = 0;
					}
				} else if(c != '\r') {
					nl = 0;
					cbuf[0] = c;
					char* tmp = header;
					header = cm_strcat(tmp, cbuf);
					free(tmp);
				}
			}
		}
	}
getout:
	free(header);
	if(bad) {
		tw_free_request(req);
		return 1;
	}
	char* result = malloc(1);
	result[0] = 0;
	int i;
	for(i = 0; req->path[i] != 0; i++) {
		if(req->path[i] == '?') {
			req->path[i] = 0;
			req->query = cm_strdup(req->path + i + 1);
			break;
		}
	}
	for(i = 0; req->path[i] != 0; i++) {
		if(req->path[i] == '%') {
			if(req->path[i + 1] == 0) continue;
			cbuf[0] = cm_hex(req->path + i + 1, 2);
			if(cbuf[0] != '\\') {
				char* tmp = result;
				result = cm_strcat(tmp, cbuf);
				free(tmp);
			}
			i += 2;
		} else if(req->path[i] != '\\') {
			cbuf[0] = req->path[i];
			char* tmp = result;
			result = cm_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	free(req->path);
	req->path = result;

	int incr = 0;
	char* p = malloc(1);
	p[0] = 0;
	int j;
	for(j = 0;; j++) {
		if(req->path[j] == '/' || req->path[j] == 0) {
			char oldc = req->path[j];
			cbuf[0] = oldc;
			req->path[j] = 0;

			char* pth = req->path + incr;

			if(strcmp(pth, "..") == 0) {
				int k;
				if(p[strlen(p) - 1] == '/') p[strlen(p) - 1] = 0;
				for(k = strlen(p) - 1; k >= 0; k--) {
					if(p[k] == '/') {
						p[k + 1] = 0;
						break;
					}
				}
				if(strlen(p) == 0) {
					free(p);
					p = cm_strdup("/");
				}
			} else if(strcmp(pth, ".") == 0) {
			} else {
				char* tmp = p;
				p = cm_strcat3(tmp, pth, cbuf);
				free(tmp);
			}

			incr = j + 1;
			if(oldc == 0) break;
		}
	}
	free(req->path);
	req->path = p;
	return 0;
}
