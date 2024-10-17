/* $Id$ */

#include <tw_module.h>

#include <cm_string.h>

int MODULE_DECL mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("CGI", "Initializing CGI module");
	tools->add_version("CGI/1.1");
	return 0;
}

int MODULE_DECL mod_config(struct tw_tool* tools, char** argv, int argc) {
	if(cm_strcaseequ(argv[0], "AllowCGI")) {
		return TW_CONFIG_PARSED;
	}
	return TW_CONFIG_NOTME;
}

int MODULE_DECL mod_request(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res) {
	res->status = 403;
	return TW_MODULE_STOP;
}

END_MODULE
