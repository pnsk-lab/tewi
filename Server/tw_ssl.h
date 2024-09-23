/* $Id$ */

#ifndef __TW_SSL_H__
#define __TW_SSL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/ssl.h>
#include <stdint.h>

SSL_CTX* tw_create_ssl_ctx(__UINTPTR_TYPE__ port);

#ifdef __cplusplus
}
#endif

#endif
