/* $Id$ */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <openssl/opensslv.h>

#include <cm_log.h>

#include "tw_config.h"
#include "tw_server.h"
#include "tw_version.h"

extern bool cm_do_log;

int main(int argc, char** argv) {
	int i;
	const char* config = PREFIX "/etc/tewi.conf";
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
				if(!cm_do_log) {
					cm_do_log = true;
					cm_log("", "This is Tewi HTTPd, version %s, using %s", tw_get_version(), OPENSSL_VERSION_TEXT);
				} else {
					cm_do_log = true;
				}
			} else if(strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-C") == 0) {
				i++;
				if(argv[i] == NULL) {
					fprintf(stderr, "Missing argument\n");
					return 1;
				}
				config = argv[i];
			} else {
				fprintf(stderr, "Unknown option: %s\n", argv[i]);
				return 1;
			}
		}
	}
	tw_config_init();
	if(tw_config_read(config) != 0) {
		fprintf(stderr, "Could not read the config\n");
		return 1;
	}
	if(tw_server_init() != 0) {
		fprintf(stderr, "Could not initialize the server\n");
		return 1;
	}
	cm_log("Daemon", "Ready");
}
