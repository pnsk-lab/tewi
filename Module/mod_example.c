/* $Id$ */

#include "../Server/tw_module.h"

int mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("Example", "This is an example module");
	tools->add_version("Example/0.0");
	return 0;
}

int mod_request(struct tw_tool* tools, struct tw_http_request* req, struct tw_http_response* res) { return TW_MODULE_ERROR(403); }
