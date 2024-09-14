/* $Id$ */

#ifndef __TW_MODULE_H__
#define __TW_MODULE_H__

#include "tw_config.h"
#include "tw_http.h"

struct tw_tool {
	void (*log)(const char* name, const char* log, ...);
	void (*add_version)(const char* string);
};

enum TW_MODULE_RETURN {
	TW_MODULE_PASS = 0, /* Pass to the next module. */
	TW_MODULE_STOP,	    /* Do not pass to the next module. */
	TW_MODULE_ERROR	    /* Error, and do not pass to the next module. */
};

typedef int (*tw_mod_init_t)(struct tw_config* config, struct tw_tool* tools);
typedef int (*tw_mod_request_t)(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res);

void* tw_module_load(const char* path);
void* tw_module_symbol(void* mod, const char* sym);
void tw_init_tools(struct tw_tool* tools);
int tw_module_init(void* mod);

#endif
