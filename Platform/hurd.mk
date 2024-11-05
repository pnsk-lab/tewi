# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -D_DEFAULT_SOURCE -fPIC -D_XOPEN_SOURCE=600
LDFLAGS =
LIBS = -ldl
EXEC =
LIBSUF = .so
