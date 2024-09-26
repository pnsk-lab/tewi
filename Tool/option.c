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
	} else if(strcmp(argv[1], "ldflags") == 0) {
#ifndef NO_SSL
		printf("-L %s/openssl/lib", argv[2]);
#endif
	} else if(strcmp(argv[1], "objs") == 0) {
#ifndef NO_SSL
		printf("ssl.o");
#endif
	} else if(strcmp(argv[1], "libs") == 0) {
#ifndef NO_SSL
		printf("-lssl -lcrypto");
#endif
	}
	printf("\n");
	return 0;
}
