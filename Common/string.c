/* $Id$ */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

char* cm_strcat(const char* a, const char* b) {
	char* str = malloc(strlen(a) + strlen(b) + 1);
	memcpy(str, a, strlen(a));
	memcpy(str + strlen(a), b, strlen(b));
	str[strlen(a) + strlen(b)] = 0;
	return str;
}

char* cm_strdup(const char* str) { return cm_strcat(str, ""); }

char* cm_trimstart(const char* str) {
	int i;
	for(i = 0; str[i] != 0; i++) {
		if(str[i] != ' ' && str[i] != '\t') {
			return cm_strdup(str + i);
		}
	}
	return cm_strdup("");
}

char* cm_trimend(const char* str) {
	char* s = cm_strdup(str);
	int i;
	for(i = strlen(s) - 1; i >= 0; i--) {
		if(s[i] != '\t' && s[i] != ' ') {
			s[i + 1] = 0;
			break;
		}
	}
	return s;
}

char* cm_trim(const char* str) {
	char* tmp = cm_trimstart(str);
	char* s = cm_trimend(tmp);
	free(tmp);
	return s;
}

char** cm_split(const char* str, const char* by) {
	int i;
	char** r = malloc(sizeof(*r));
	r[0] = NULL;
	char* b = malloc(1);
	b[0] = 0;
	char cbuf[2];
	cbuf[1] = 0;
	bool dq = false;
	bool sq = false;
	for(i = 0;; i++) {
		int j;
		bool has = false;
		for(j = 0; by[j] != 0; j++) {
			if(by[j] == str[i]) {
				has = true;
				break;
			}
		}
		if(!(dq || sq) && (has || str[i] == 0)) {
			if(strlen(b) > 0) {
				char** old = r;
				int j;
				for(j = 0; old[j] != NULL; j++)
					;
				r = malloc(sizeof(*r) * (j + 2));
				for(j = 0; old[j] != NULL; j++) r[j] = old[j];
				r[j] = b;
				r[j + 1] = NULL;
				free(old);
			}
			b = malloc(1);
			b[0] = 0;
			if(str[i] == 0) break;
		} else {
			if(str[i] == '"' && !sq) {
				dq = !dq;
			} else if(str[i] == '\'' && !dq) {
				sq = !sq;
			} else {
				cbuf[0] = str[i];
				char* tmp = b;
				b = cm_strcat(tmp, cbuf);
				free(tmp);
			}
		}
	}
	free(b);
	return r;
}

bool cm_strcaseequ(const char* a, const char* b) {
	if(a == NULL) return false;
	if(b == NULL) return false;
	if(strlen(a) != strlen(b)) return false;
	int i;
	for(i = 0; a[i] != 0; i++) {
		if(tolower(a[i]) != tolower(b[i])) return false;
	}
	return true;
}
