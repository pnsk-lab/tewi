/* $Id$ */

#ifndef __TW_CONFIG_H__
#define __TW_CONFIG_H__

#include <stdint.h>

#define MAX_PORTS 1024

#define MAX_VHOSTS 1024

struct tw_config_entry {
	char* name;
	int port;
	char* sslkey;
	char* sslcert;
};

struct tw_config {
	uint64_t ports[MAX_PORTS + 1]; /* If port & (1 << 32) is non-zero, it is SSL */
	char hostname[1025];
	struct tw_config_entry root;
	struct tw_config_entry vhosts[MAX_VHOSTS];
	int vhost_count;
};

void tw_config_init(void);
int tw_config_read(const char* path);
struct tw_config_entry* tw_vhost_match(const char* name, int port);

#endif
