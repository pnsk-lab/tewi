/* $Id$ */

#include "tw_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cm_string.h>
#include <cm_log.h>

struct tw_config config;

void tw_config_init(void) {}

int tw_config_read(const char* path) {
	cm_log("Config", "Reading %s", path);
	char cbuf[2];
	cbuf[1] = 0;
	int ln = 0;
	FILE* f = fopen(path, "r");
	if(f != NULL) {
		char* line = malloc(1);
		line[0] = 0;
		int stop = 0;
		char* vhost = NULL;
		while(stop == 0) {
			int c = fread(cbuf, 1, 1, f);
			if(cbuf[0] == '\n' || c <= 0) {
				ln++;
				char* l = cm_trim(line);
				if(strlen(l) > 0 && l[0] != '#') {
					char** r = cm_split(l, " \t");
					int i;
					if(cm_strcaseequ(r[0], "Include") || cm_strcaseequ(r[0], "IncludeOptional")) {
						for(i = 1; r[i] != NULL; i++) {
							if(tw_config_read(r[i]) != 0 && cm_strcaseequ(r[0], "Include")) {
								stop = 1;
								break;
							}
						}
					} else if(cm_strcaseequ(r[0], "BeginVirtualHost")) {
						if(vhost != NULL) {
							cm_log("Config", "Already in virtual host section");
							stop = 1;
						} else {
							if(r[1] == NULL) {
								cm_log("Config", "Missing virtual host");
								stop = 1;
							} else {
								vhost = cm_strdup(r[1]);
							}
						}
					} else if(cm_strcaseequ(r[0], "EndVirtualHost")) {
						if(vhost == NULL) {
							cm_log("Config", "Not in virtual host section");
							stop = 1;
						} else {
							free(vhost);
							vhost = NULL;
						}
					} else {
						if(r[0] != NULL) {
							cm_log("Config", "Unknown directive `%s' at line %d", r[0], ln);
						}
						stop = 1;
					}
					for(i = 0; r[i] != NULL; i++) free(r[i]);
					free(r);
				}
				free(l);
				free(line);
				line = malloc(1);
				line[0] = 0;
				if(c <= 0) break;
			} else if(cbuf[0] != '\r') {
				char* tmp = line;
				line = cm_strcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(line);
		fclose(f);
		return stop;
	} else {
		cm_log("Config", "Could not open the file");
		return 1;
	}
}
