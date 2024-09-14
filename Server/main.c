/* $Id$ */

#define SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <openssl/opensslv.h>

#include <cm_log.h>

#include "tw_config.h"
#include "tw_server.h"
#include "tw_version.h"

extern bool cm_do_log;
extern struct tw_config config;

char tw_server[2048];

int main(int argc, char** argv) {
	int i;
	const char* confpath = PREFIX "/etc/tewi.conf";
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
				confpath = argv[i];
			} else {
				fprintf(stderr, "Unknown option: %s\n", argv[i]);
				return 1;
			}
		}
	}
	tw_config_init();
	if(tw_config_read(confpath) != 0) {
		fprintf(stderr, "Could not read the config\n");
		return 1;
	}
	if(tw_server_init() != 0) {
		fprintf(stderr, "Could not initialize the server\n");
		return 1;
	}
	sprintf(tw_server, "Tewi/%s (%s)%s", tw_get_version(), tw_get_platform(), config.extension == NULL ? "" : config.extension);
	cm_log("Daemon", "Ready");
#ifndef __MINGW32__
	signal(SIGCHLD, SIG_IGN);
#endif
	tw_server_loop();
}
