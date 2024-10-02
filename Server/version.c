/* $Id$ */

#define SOURCE

#include "tw_version.h"

const char* tw_version = TW_VERSION;

const char* tw_platform =
#if defined(PLATFORM)
    PLATFORM
#elif defined(__NetBSD__)
    "NetBSD"
#elif defined(__linux__)
    "Linux"
#elif defined(__MINGW32__)
    "Windows-MinGW32"
#elif defined(_MSC_VER)
    "Windows-MSVC"
#elif defined(__BORLANDC__)
    "Windows-BCC"
#elif defined(__HAIKU__)
    "Haiku"
#elif defined(__CYGWIN__)
    "Cygwin"
#elif defined(_PSP)
    "PSP"
#elif defined(__PPU__)
    "PS3"
#else
    "Unix"
#endif
    ;

const char* tw_get_version(void) { return tw_version; }
const char* tw_get_platform(void) { return tw_platform; }
