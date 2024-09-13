/* $Id$ */

#ifndef __TW_HTTP_H__
#define __TW_HTTP_H__

struct tw_http_request {
	char* method;
	char* path;
	char* version;
	char** headers;
	char* body;
};

struct tw_http_response {
	char** headers;
};

struct tw_http_tool {};

#ifdef SOURCE
#include <openssl/ssl.h>
int tw_http_parse(SSL* ssl, int sock, struct tw_http_request* req);
#endif

#endif
