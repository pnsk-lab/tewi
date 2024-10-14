/* $Id$ */

#include "cm_dir.h"

#include "cm_string.h"

#include <sys/stat.h>
#if !defined(_MSC_VER) && !defined(__WATCOMC__)
#include <dirent.h>
#elif defined(__NETWARE__)
#include <dirent.h>
#elif defined(__WATCOMC__) || defined(_MSC_VER)
#include <direct.h>
#endif
#ifdef _MSC_VER
#include <windows.h>
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
	WIN32_FIND_DATA ffd;
	HANDLE hfind;
	char** r = malloc(sizeof(*r));
	int len;
	char** old;
	int i;
	char* p;
	r[0] = NULL;

	p = cm_strcat(path, "/*");
	hfind = FindFirstFile(p, &ffd);
	if(INVALID_HANDLE_VALUE == hfind) {
		return NULL;
	}
	do {
		if(strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
			old = r;
			for(i = 0; old[i] != NULL; i++)
				;
			r = malloc(sizeof(*r) * (i + 2));
			for(i = 0; old[i] != NULL; i++) r[i] = old[i];
			r[i] = cm_strcat(ffd.cFileName, (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "/" : "");
			r[i + 1] = NULL;
			free(old);
		}
	} while(FindNextFile(hfind, &ffd) != 0);
	FindClose(hfind);
	free(p);
	for(len = 0; r[len] != NULL; len++)
		;
	qsort(r, len, sizeof(char*), cm_sort);

	old = r;
	for(i = 0; old[i] != NULL; i++)
		;
	r = malloc(sizeof(*r) * (i + 2));
	for(i = 0; old[i] != NULL; i++) r[i + 1] = old[i];
	r[0] = cm_strdup("../");
	r[i + 1] = NULL;
	free(old);

	return r;
#else
	DIR* dir = opendir(path);
	if(dir != NULL) {
		char** r = malloc(sizeof(*r));
		struct dirent* d;
		char** old;
		int len;
		int i;
		r[0] = NULL;
		while((d = readdir(dir)) != NULL) {
			if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
				struct stat s;
				char* p = cm_strcat3(path, "/", d->d_name);
				stat(p, &s);
				free(p);

				old = r;
				for(i = 0; old[i] != NULL; i++)
					;
				r = malloc(sizeof(*r) * (i + 2));
				for(i = 0; old[i] != NULL; i++) r[i] = old[i];
				r[i] = cm_strcat(d->d_name, S_ISDIR(s.st_mode) ? "/" : "");
				r[i + 1] = NULL;
				free(old);
			}
		}
		for(len = 0; r[len] != NULL; len++)
			;
		qsort(r, len, sizeof(char*), cm_sort);

		old = r;
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
