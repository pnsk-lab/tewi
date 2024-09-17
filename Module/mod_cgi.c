/* $Id$ */

#include "../Server/tw_module.h"

int mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("CGI", "Initializing CGI module");
	tools->add_version("CGI/1.1");
	return 0;
}

int mod_config(struct tw_tool* tools, char** argv, int argc) {
	printf("args %d\n", argc);
	return TW_CONFIG_ERROR;
}

int mod_request(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res) { return TW_MODULE_PASS; }
