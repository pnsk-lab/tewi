# $Id$

CC = cc
AR = ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I ../VC6Compat -I $(PWD)/Common -D_POSIX_SOURCE -D_NEXT_SOURCE -Duint64_t=uint32_t -DREALLY_OLD
LDFLAGS =
LIBS =
EXEC =
LIBSUF = .so
AR_USUAL=rc
MODULE =
MAYBE_RANLIB=ranlib common.a
