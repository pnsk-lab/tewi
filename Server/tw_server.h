/* $Id$ */

#ifndef __TW_SERVER_H__
#define __TW_SERVER_H__

#include <openssl/ssl.h>

int tw_server_init(void);
void tw_server_loop(void);
size_t tw_read(SSL* ssl, int s, void* data, size_t len);
size_t tw_write(SSL* ssl, int s, void* data, size_t len);

#endif
