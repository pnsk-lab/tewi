/* $Id$ */

#include "../Server/tw_module.h"

#include <cm_string.h>

int mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("CGI", "Initializing CGI module");
	tools->add_version("CGI/1.1");
	return 0;
}

int mod_config(struct tw_tool* tools, char** argv, int argc) {
	if(cm_strcaseequ(argv[0], "AllowCGI")) {
		return TW_CONFIG_PARSED;
	}
	return TW_CONFIG_NOTME;
}

int mod_request(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res) { return TW_MODULE_PASS; }
