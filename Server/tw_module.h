/* $Id$ */

#ifndef __TW_MODULE_H__
#define __TW_MODULE_H__

#include "tw_config.h"

struct tw_tool {
	void (*log)(const char* name, const char* log, ...);
};

typedef int (*tw_mod_init_t)(struct tw_config* config, struct tw_tool* tools);

void* tw_module_load(const char* path);
void* tw_module_symbol(void* mod, const char* sym);
void tw_init_tools(struct tw_tool* tools);
int tw_module_init(void* mod);

#endif
