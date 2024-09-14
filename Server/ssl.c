/* $Id$ */

#define SOURCE

#include "tw_ssl.h"

#include "tw_config.h"

#include <stdio.h>

#include <cm_log.h>

extern struct tw_config config;

int tw_ssl_cert_cb(SSL* ssl, void* arg) {
	const char* s = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
	if(s != NULL) {
		cm_log("SSL", "Certificate request for %s", s);
	} else {
		s = config.hostname;
		cm_log("SSL", "Could not get the servername, defaulting to the hostname: %s", s);
	}
	struct tw_config_entry* e = tw_vhost_match(s, (uint64_t)arg);
	if(e != NULL && e->sslkey != NULL && e->sslcert != NULL) {
		SSL_use_PrivateKey_file(ssl, e->sslkey, SSL_FILETYPE_PEM);
		SSL_use_certificate_file(ssl, e->sslcert, SSL_FILETYPE_PEM);
		return 1;
	} else if(config.root.sslkey != NULL && config.root.sslcert != NULL) {
		SSL_use_PrivateKey_file(ssl, config.root.sslkey, SSL_FILETYPE_PEM);
		SSL_use_certificate_file(ssl, config.root.sslcert, SSL_FILETYPE_PEM);
		return 1;
	} else {
		return 0;
	}
}

SSL_CTX* tw_create_ssl_ctx(uint64_t port) {
	SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
	SSL_CTX_set_cert_cb(ctx, tw_ssl_cert_cb, (void*)port);
	return ctx;
}
