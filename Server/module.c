/* $Id$ */

#define SOURCE

#include "tw_module.h"

#include "tw_config.h"

#include <cm_string.h>
#include <cm_log.h>

#include <string.h>
#include <stdlib.h>
#if !defined(_MSC_VER) && !defined(__BORLANDC__)
#include <unistd.h>
#endif

extern struct tw_config config;

#if defined(_PSP) || defined(__PPU__) || defined(__ps2sdk__)
void* tw_module_load(const char* path) { return NULL; }

void* tw_module_symbol(void* mod, const char* sym) { return NULL; }

int tw_module_init(void* mod) { return 1; }

#else

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#ifdef __OS2__
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#include <os2.h>
#elif defined(__NETWARE__)
#include <nwadv.h>
#include <nwthread.h>
#else
#include <windows.h>
#include <direct.h>
#endif
#else
#include <dlfcn.h>
#endif

void* tw_module_load(const char* path) {
	char* p = getcwd(NULL, 0);
	void* lib;
	char tmp[512];
#ifdef __OS2__
	HMODULE mod;
#elif defined(__NETWARE__)
	unsigned int* hnd = malloc(sizeof(*hnd));
#endif
	chdir(config.server_root);
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#ifdef __OS2__
	if(DosLoadModule(tmp, 512, path, &mod) != NO_ERROR) {
		return NULL;
	}
	lib = (void*)mod;
#elif defined(__NETWARE__)
	*hnd = FindNLMHandle(path);
	lib = (void*)hnd;
#else
	lib = LoadLibraryA(path);
#endif
#else
	lib = dlopen(path, RTLD_LAZY);
#endif
	if(lib == NULL) {
		cm_log("Module", "Could not load %s", path);
	}
	chdir(p);
	free(p);
	return lib;
}

void* tw_module_symbol(void* mod, const char* sym) {
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
#ifdef __OS2__
	void* ret;
	APIRET rc;
	if((rc = DosQueryProcAddr((HMODULE)mod, 0, sym, (PFN*)&ret)) != NO_ERROR) {
		cm_log("Module", "OS/2 error %d", (int)rc);
		return NULL;
	}
	return ret;
#elif defined(__NETWARE__)
	return ImportSymbol(*(unsigned int*)mod, sym);
#else
	return GetProcAddress(mod, sym);
#endif
#else
	return dlsym(mod, sym);
#endif
}

int tw_module_init(void* mod) {
	tw_mod_init_t mod_init = (tw_mod_init_t)tw_module_symbol(mod, "mod_init");
	if(mod_init == NULL) {
		cm_log("Module", "Could not find a init call");
		return 1;
	} else {
		struct tw_tool tools;
		tw_init_tools(&tools);
		return mod_init(&config, &tools);
	}
}
#endif

void tw_add_version(const char* string) {
	if(config.extension == NULL) {
		config.extension = cm_strcat(" ", string);
	} else {
		char* tmp = config.extension;
		config.extension = cm_strcat3(tmp, " ", string);
		free(tmp);
	}
}

void tw_add_define(const char* string) {
	int i;
	for(i = 0; config.defined[i] != NULL; i++) {
		if(strcmp(config.defined[i], string) == 0) {
			return;
		}
	}
	for(i = 0; config.defined[i] != NULL; i++)
		;
	config.defined[i] = cm_strdup(string);
	config.defined[i + 1] = NULL;
}

void tw_delete_define(const char* string) {
	int i;
	for(i = 0; config.defined[i] != NULL; i++) {
		if(strcmp(config.defined[i], string) == 0) {
			free(config.defined[i]);
			for(; config.defined[i] != NULL; i++) {
				config.defined[i] = config.defined[i + 1];
			}
			break;
		}
	}
}

void tw_init_tools(struct tw_tool* tools) {
	tools->log = cm_log;
	tools->add_version = tw_add_version;
	tools->add_define = tw_add_define;
}
