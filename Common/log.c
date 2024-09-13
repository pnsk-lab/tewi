/* $Id$ */

#include "cm_log.h"

#include "cm_string.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

bool cm_do_log = false;

#define LOGNAME_LENGTH 12

void cm_log(const char* name, const char* log, ...) {
	if(!cm_do_log) return;
	va_list args;
	va_start(args, log);
	char namebuf[LOGNAME_LENGTH + 1];
	memset(namebuf, '.', LOGNAME_LENGTH);
	namebuf[LOGNAME_LENGTH] = 0;
	int i;
	for(i = 0; name[i] != 0 && i < LOGNAME_LENGTH; i++) {
		namebuf[i] = name[i];
	}

	char* result = malloc(1);
	result[0] = 0;

	char cbuf[2];
	cbuf[1] = 0;

	for(i = 0; log[i] != 0; i++) {
		if(log[i] == '%') {
			i++;
			if(log[i] == 's') {
				char* tmp = result;
				result = cm_strcat(tmp, va_arg(args, char*));
				free(tmp);
			}
		} else {
			cbuf[0] = log[i];
			char* tmp = result;
			result = cm_strcat(tmp, cbuf);
			free(tmp);
		}
	}

	fprintf(stderr, "%s %s\n", namebuf, result);
	va_end(args);

	free(result);
}
