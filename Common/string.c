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

char* cm_trimstart(const char* str){
	int i;
	for(i = 0; str[i] != 0; i++){
		if(str[i] != ' ' && str[i] != '\t'){
			return cm_strdup(str + i);
		}
	}
	return cm_strdup("");
}

char* cm_trimend(const char* str){
	char* s = cm_strdup(str);
	int i;
	for(i = strlen(s) - 1; i >= 0; i--){
		if(s[i] != '\t' && s[i] != ' '){
			s[i + 1] = 0;
			break;
		}
	}
	return s;
}

char* cm_trim(const char* str){
	char* tmp = cm_trimstart(str);
	char* s = cm_trimend(tmp);
	free(tmp);
	return s;
}
