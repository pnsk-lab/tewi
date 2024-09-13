/* $Id$ */

#include "tw_module.h"

#include "tw_config.h"

#include <cm_string.h>
#include <cm_log.h>

#include <unistd.h>
#include <stdlib.h>

#ifdef __MINGW32__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

extern struct tw_config config;

void* tw_module_load(const char* path) {
	char* p = getcwd(NULL, 0);
	chdir(config.server_root);
	void* lib;
#ifdef __MINGW32__
	lib = LoadLibraryA(path);
#else
	lib = dlopen(path, DL_LAZY);
#endif
	if(lib == NULL) {
		cm_log("Module", "Could not load %s", path);
	}
	chdir(p);
	free(p);
	return lib;
}

void* tw_module_symbol(void* mod, const char* sym) {
#ifdef __MINGW32__
	return GetProcAddress(mod, sym);
#else
	return dlsym(mod, sym);
#endif
}

void tw_init_tools(struct tw_tool* tools) { tools->log = cm_log; }

int tw_module_init(void* mod) {
	tw_mod_init_t mod_init = (tw_mod_init_t)tw_module_symbol(mod, "mod_init");
	if(mod_init == NULL) {
		cm_log("Module", "Could not init a module");
		return 1;
	} else {
		struct tw_tool tools;
		tw_init_tools(&tools);
		return mod_init(&config, &tools);
	}
}
