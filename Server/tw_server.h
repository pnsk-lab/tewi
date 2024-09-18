/* $Id$ */

#ifndef __TW_SERVER_H__
#define __TW_SERVER_H__

#include "../config.h"

#include <stddef.h>

#ifndef NO_SSL
#include <openssl/ssl.h>
#endif

int tw_server_init(void);
void tw_server_loop(void);

#ifndef NO_SSL
size_t tw_read(SSL* ssl, int s, void* data, size_t len);
size_t tw_write(SSL* ssl, int s, void* data, size_t len);
#else
size_t tw_read(void* ssl, int s, void* data, size_t len);
size_t tw_write(void* ssl, int s, void* data, size_t len);
#endif

#endif
