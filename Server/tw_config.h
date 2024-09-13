/* $Id$ */

#ifndef __TW_CONFIG_H__
#define __TW_CONFIG_H__

struct tw_config_entry {};

struct tw_config {
	struct tw_config_entry root;
};

void tw_config_init(void);
int tw_config_read(const char* path);

#endif
