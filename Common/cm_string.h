/* $Id$ */

#ifndef __CM_STRING_H__
#define __CM_STRING_H__

#include <stdbool.h>

int cm_hex(const char* str, int len);
char* cm_html_escape(const char* str);
char* cm_url_escape(const char* str);
char* cm_strcat(const char* a, const char* b);
char* cm_strcat3(const char* a, const char* b, const char* c);
char* cm_strdup(const char* str);
char* cm_trimstart(const char* str);
char* cm_trimend(const char* str);
char* cm_trim(const char* str);
char** cm_split(const char* str, const char* by);
bool cm_strcaseequ(const char* a, const char* b);

#endif
