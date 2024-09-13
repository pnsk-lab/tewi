/* $Id$ */

#include "tw_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cm_string.h>
#include <cm_log.h>

int tw_config_read(const char* path){
	cm_log("Config", "Reading %s", path);
	char cbuf[2];
	cbuf[1] = 0;
	FILE* f = fopen(path, "r");
	if(f != NULL){
		char* line = malloc(1);
		line[0] = 0;
		while(1){
			int c = fread(cbuf, 1, 1, f);
			if(cbuf[0] == '\n' || c <= 0){
				char* l = cm_trim(line);
				if(strlen(l) > 0 && l[0] != '#'){
					printf("[%s]\n", l);
				}
				free(l);
				free(line);
				line = malloc(1);
				line[0] = 0;
				if(c <= 0) break;
			}else if(cbuf[0] != '\r'){
				char* tmp = line;
				line = cm_strcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(line);
		fclose(f);
		return 0;
	}else{
		return 1;
	}
}
