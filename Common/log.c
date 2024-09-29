/* $Id$ */

#include "cm_log.h"

#include "cm_string.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef _PSP
#include <pspdebug.h>
#endif

#ifdef __PPU__
extern void tt_printf(const char* tmpl, ...);
#endif

FILE* logfile;

bool cm_do_log = false;

#define LOGNAME_LENGTH 12

void cm_force_log(const char* log) {
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	char date[513];
	strftime(date, 512, "%a %b %d %H:%M:%S %Z %Y", tm);
#ifdef _PSP
	pspDebugScreenPrintf("[%s] %s\n", date, log);
#elif defined(__PPU__)
	tt_printf("[%s] %s\n", date, log);
#else
	fprintf(logfile, "[%s] %s\n", date, log);
#endif
	fflush(logfile);
}

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
				char* c = va_arg(args, char*);
				result = cm_strcat(tmp, c == NULL ? "(null)" : c);
				free(tmp);
			} else if(log[i] == 'd') {
				int a = va_arg(args, int);
				char buf[128];
				sprintf(buf, "%d", a);
				char* tmp = result;
				result = cm_strcat(tmp, buf);
				free(tmp);
			}
		} else {
			cbuf[0] = log[i];
			char* tmp = result;
			result = cm_strcat(tmp, cbuf);
			free(tmp);
		}
	}

#ifdef _PSP
	pspDebugScreenPrintf("%s %s\n", namebuf, result);
#elif defined(__PPU__)
	tt_printf("%s %s\n", namebuf, result);
#else
	fprintf(logfile, "%s %s\n", namebuf, result);
#endif
	va_end(args);

	free(result);
}
