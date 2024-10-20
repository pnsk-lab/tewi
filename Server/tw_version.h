/* $Id$ */

#ifndef __TW_VERSION_H__
#define __TW_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TW_VERSION "2.05A\0"
#define TW_VERSION_TEXT "Tewi HTTPd version 2.05A"

const char* tw_get_version(void);
const char* tw_get_platform(void);

#ifdef __cplusplus
}
#endif

#endif
