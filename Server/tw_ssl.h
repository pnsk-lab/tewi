/* $Id$ */

#ifndef __TW_SSL_H__
#define __TW_SSL_H__

#include <openssl/ssl.h>
#include <stdint.h>

SSL_CTX* tw_create_ssl_ctx(__UINTPTR_TYPE__ port);

#endif
