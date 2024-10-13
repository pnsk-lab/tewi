/* $Id$ */

#define SOURCE

#include "tw_version.h"

const char* tw_version = TW_VERSION;

const char* tw_platform =
#if defined(PLATFORM)
    PLATFORM
#elif defined(__NetBSD__)
    "NetBSD"
#elif defined(__OpenBSD__)
    "OpenBSD"
#elif defined(__linux__)
    "Linux"
#elif defined(__OS2__)
    "OS/2"
#elif defined(__MINGW32__)
    "Windows-MinGW32"
#elif defined(_MSC_VER)
    "Windows-VisualC"
#elif defined(__BORLANDC__)
    "Windows-Borland"
#elif defined(__WATCOMC__)
    "Windows-Watcom"
#elif defined(__HAIKU__)
    "Haiku"
#elif defined(__CYGWIN__)
    "Cygwin"
#elif defined(_PSP)
    "PSP"
#elif defined(__PPU__)
    "PS3"
#elif defined(__minix)
    "Minix"
#elif defined(__USLC__)
    "UnixWare"
#else
    "Unix"
#endif
    ;

const char* tw_get_version(void) { return tw_version; }
const char* tw_get_platform(void) { return tw_platform; }
