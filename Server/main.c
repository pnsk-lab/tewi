/* $Id$ */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <cm_log.h>

#include "tw_version.h"

extern bool cm_do_log;

int main(int argc, char** argv) {
	int i;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
				if(!cm_do_log) {
					cm_do_log = true;
					cm_log("", "This is Tewi HTTPd, version %s", tw_get_version());
				} else {
					cm_do_log = true;
				}
			} else {
				fprintf(stderr, "Unknown option: %s\n", argv[i]);
				return 1;
			}
		}
	}
	cm_log("Daemon", "Ready");
}
