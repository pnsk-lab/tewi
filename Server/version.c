/* $Id$ */

#define SOURCE

#include "tw_version.h"

const char* tw_version = "0.00";

const char* tw_platform =
#if defined(PLATFORM)
    PLATFORM
#elif defined(__NetBSD__)
    "NetBSD"
#elif defined(__MINGW32__)
    "Windows"
#else
    "Unix"
#endif
    ;

const char* tw_get_version(void) { return tw_version; }
const char* tw_get_platform(void) { return tw_platform; }
