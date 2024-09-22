/* $Id$ */

#include "../Server/tw_module.h"

#include <cm_string.h>

int mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("CGI", "Initializing Proxy module");
	tools->add_version("Proxy/1.0");
	return 0;
}

int mod_config(struct tw_tool* tools, char** argv, int argc) { return TW_CONFIG_NOTME; }

int mod_request(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res) { return TW_MODULE_PASS; }
