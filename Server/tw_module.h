/* $Id$ */

#ifndef __TW_MODULE_H__
#define __TW_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tw_config.h"
#include "tw_http.h"

#if defined(__OS2__)
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#include <os2.h>
#define MODULE_DECL __export APIENTRY
#else
#define MODULE_DECL
#endif

struct tw_tool {
	void (*log)(const char* name, const char* log, ...);
	void (*add_version)(const char* string);
	void (*add_define)(const char* string);
	void (*delete_define)(const char* string);
};

enum TW_MODULE_RETURN {
	_TW_MODULE_PASS = 0, /* Pass to the next module. */
	_TW_MODULE_STOP,     /* Do not pass to the next module. */
	_TW_MODULE_STOP2,    /* Do not pass to the next module, and do not handle response. */
	_TW_MODULE_ERROR,    /* Error, and do not pass to the next module. */

	_TW_CONFIG_PARSED, /* Got parsed */
	_TW_CONFIG_NOTME,  /* Did not parse */
	_TW_CONFIG_ERROR   /* Error */
};

#define TW_MODULE_PASS _TW_MODULE_PASS
#define TW_MODULE_STOP _TW_MODULE_STOP
#define TW_MODULE_STOP2 _TW_MODULE_STOP2
#define TW_MODULE_ERROR(x) (_TW_MODULE_ERROR | ((x) << 8))

#define TW_CONFIG_PARSED _TW_CONFIG_PARSED
#define TW_CONFIG_NOTME _TW_CONFIG_NOTME
#define TW_CONFIG_ERROR _TW_CONFIG_ERROR

typedef int (MODULE_DECL *tw_mod_init_t)(struct tw_config* config, struct tw_tool* tools);
typedef int (MODULE_DECL *tw_mod_request_t)(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res);
typedef int (MODULE_DECL *tw_mod_config_t)(struct tw_tool* tools, char** argv, int argc);

#ifdef SOURCE
void* tw_module_load(const char* path);
void* tw_module_symbol(void* mod, const char* sym);
void tw_init_tools(struct tw_tool* tools);
int tw_module_init(void* mod);

void tw_add_version(const char* string);
void tw_add_define(const char* string);
void tw_delete_define(const char* string);
#endif

#ifdef __cplusplus
}
#endif

#endif
