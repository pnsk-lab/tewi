/* $Id$ */

#include <string.h>
#include <stdlib.h>

char* cm_strcat(const char* a, const char* b) {
	char* str = malloc(strlen(a) + strlen(b) + 1);
	memcpy(str, a, strlen(a));
	memcpy(str + strlen(a), b, strlen(b));
	str[strlen(a) + strlen(b)] = 0;
	return str;
}

char* cm_strdup(const char* str) { return cm_strcat(str, ""); }
