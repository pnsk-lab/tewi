/* $Id$ */

#ifndef __TW_CONFIG_H__
#define __TW_CONFIG_H__

#include <stdint.h>

/* I don't think you would listen to 1024 ports */
#define MAX_PORTS 1024

struct tw_config_entry {};

struct tw_config {
	uint64_t ports[MAX_PORTS + 1]; /* If port & (1 << 32) is non-zero, it is SSL */
	struct tw_config_entry root;
};

void tw_config_init(void);
int tw_config_read(const char* path);

#endif
