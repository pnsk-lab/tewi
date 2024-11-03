/* $Id$ */

#ifndef __STDINT_H__
#define __STDINT_H__

#ifdef __bsdi__
#include <machine/types.h>
typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;

#define INT32_MAX 0x7fffffff
#elif defined(__sun__)
#include <sys/int_types.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

#define INT32_MAX 0x7fffffff
#endif

#endif
