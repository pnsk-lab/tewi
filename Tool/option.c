/* $Id$ */
/* This file is not intended to be in the server. */

#include <stdio.h>
#include <string.h>

#include "../config.h"

int main(int argc, char** argv) {
	if(argc < 3) {
		return 1;
	}
	if(strcmp(argv[1], "cflags") == 0) {
#ifndef NO_SSL
		printf("-I %s/openssl/include", argv[2]);
#endif
#ifdef BUILD_GUI
		if(strcmp(argv[3], "WINDOWS_WATCOM") == 0){
			printf(" -b nt_win");
		}else if(strcmp(argv[3], "WINDOWS") == 0){
			printf(" -mwindows");
		}
#endif
	} else if(strcmp(argv[1], "ldflags") == 0) {
#ifndef NO_SSL
		printf("-L %s/openssl/lib", argv[2]);
#endif
#ifdef BUILD_GUI
		if(strcmp(argv[3], "WINDOWS_WATCOM") == 0){
			printf(" -b nt_win");
		}else if(strcmp(argv[3], "WINDOWS") == 0){
			printf(" -mwindows");
		}
#endif
	} else if(strcmp(argv[1], "objs") == 0) {
#ifndef NO_SSL
		printf("ssl.%s", argv[4]);
#endif
#ifdef BUILD_GUI
		if(strcmp(argv[3], "WINDOWS") == 0 || strcmp(argv[3], "WINDOWS_WATCOM") == 0){
			printf(" gui.%s", argv[4]);
		}
#endif
	} else if(strcmp(argv[1], "libs") == 0) {
#ifndef NO_SSL
		printf("-lssl -lcrypto");
#endif
		if(strcmp(argv[3], "WINDOWS") == 0){
#ifdef USE_WINSOCK1
			printf(" -lwsock32");
#else
			printf(" -lws2_32");
#endif
#ifdef BUILD_GUI
			printf(" -lcomctl32");
#endif
		}else if(strcmp(argv[3], "WINDOWS_WATCOM") == 0){
#ifdef USE_WINSOCK1
			printf(" wsock32.lib");
#else
			printf(" ws2_32.lib");
#endif
#ifdef BUILD_GUI
			printf(" comctl32.lib");
#endif
		}
	}
	printf("\n");
	return 0;
}
