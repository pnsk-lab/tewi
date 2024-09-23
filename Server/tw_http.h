/* $Id$ */

#ifndef __TW_HTTP_H__
#define __TW_HTTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../config.h"

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
#ifndef NO_SSL
#include <openssl/ssl.h>
#endif
void tw_free_request(struct tw_http_request* req);
#ifndef NO_SSL
int tw_http_parse(SSL* ssl, int sock, struct tw_http_request* req);
#else
int tw_http_parse(void* ssl, int sock, struct tw_http_request* req);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
