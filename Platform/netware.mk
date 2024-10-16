# $Id$

PREFIX = SYS:/Tewi

PLATFORM_IDENT = NETWARE_WATCOM
CC = owcc
AR = wlib
AR_FLAGS = -q -b -n -fo
CFLAGS = -b netware_clib_lite -I $(NOVELLNDK)/include -I $(NOVELLNDK)/include/nlm -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -I ../VC6Compat -fPIC -D__WATCOM_LFN__ -Duint64_t=uint32_t
LDFLAGS = -b netware_clib_lite
LIBS =
EXEC = .nlm
STATIC = lib
LIBSUF = .nlm
OBJ = obj
