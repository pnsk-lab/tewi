/* $Id$ */

#ifndef __TW_SSL_H__
#define __TW_SSL_H__

#include <openssl/ssl.h>

SSL_CTX* tw_create_ssl_ctx(uint64_t port);

#endif
