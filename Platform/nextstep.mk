# $Id$

CC = cc
AR = ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I ../VC6Compat -I $(PWD)/Common -D_POSIX_SOURCE -D_XOPEN_SOURCE=600
LDFLAGS =
LIBS = -ldl
EXEC =
LIBSUF = .so
AR_USUAL=rc
MAYBE_RANLIB=ranlib common.a
