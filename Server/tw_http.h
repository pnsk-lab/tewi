/* $Id$ */

#ifndef __TW_HTTP_H__
#define __TW_HTTP_H__

#include <stdbool.h>

struct tw_http_request {
	char* method;
	char* path;
	char* query;
	char* version;
	char** headers;
	char* body;
};

struct tw_http_response {
	char** headers;
	int status;
	bool _processed; /* Internal parameter */
};

#ifdef SOURCE
#include <openssl/ssl.h>
int tw_http_parse(SSL* ssl, int sock, struct tw_http_request* req);
#endif

#endif
