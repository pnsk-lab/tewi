/* $Id$ */

#include "cm_dir.h"

#include "cm_string.h"

#include <sys/stat.h>
#if !defined(_MSC_VER) && !defined(__WATCOMC__)
#include <dirent.h>
#elif defined(__WATCOMC__)
#include <direct.h>
#endif
#include <stdlib.h>
#include <string.h>

int cm_sort(const void* _a, const void* _b) {
	char* a = *(char**)_a;
	char* b = *(char**)_b;
	return strcmp(a, b);
}

char** cm_scandir(const char* path) {
#if defined(_MSC_VER) || defined(__BORLANDC__)
	return NULL;
#else
	DIR* dir = opendir(path);
	if(dir != NULL) {
		char** r = malloc(sizeof(*r));
		struct dirent* d;
		r[0] = NULL;
		while((d = readdir(dir)) != NULL) {
			if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
				struct stat s;
				char* p = cm_strcat3(path, "/", d->d_name);
				stat(p, &s);
				free(p);

				char** old = r;
				int i;
				for(i = 0; old[i] != NULL; i++)
					;
				r = malloc(sizeof(*r) * (i + 2));
				for(i = 0; old[i] != NULL; i++) r[i] = old[i];
				r[i] = cm_strcat(d->d_name, S_ISDIR(s.st_mode) ? "/" : "");
				r[i + 1] = NULL;
				free(old);
			}
		}
		int len;
		for(len = 0; r[len] != NULL; len++)
			;
		qsort(r, len, sizeof(char*), cm_sort);

		char** old = r;
		int i;
		for(i = 0; old[i] != NULL; i++)
			;
		r = malloc(sizeof(*r) * (i + 2));
		for(i = 0; old[i] != NULL; i++) r[i + 1] = old[i];
		r[0] = cm_strdup("../");
		r[i + 1] = NULL;
		free(old);

		closedir(dir);

		return r;
	} else {
		return NULL;
	}
#endif
}
