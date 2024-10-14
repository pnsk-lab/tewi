#ifndef STRPTIME_H
#define STRPTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#if defined(_WIN32) || defined(_PSP) || defined(__ps2sdk__) || defined(__OS2__) || defined(__NETWARE__)
char* strptime(const char *buf, const char *fmt, struct tm *tm);
#endif

#ifdef __cplusplus
}
#endif

#endif // STRPTIME_H
