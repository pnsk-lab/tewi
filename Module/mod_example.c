/* $Id$ */

#include "../Server/tw_module.h"

int mod_init(struct tw_config* config, struct tw_tool* tools) {
	tools->log("Example", "This is an example module");
	tools->add_version("Example/0.0");
	return 0;
}
