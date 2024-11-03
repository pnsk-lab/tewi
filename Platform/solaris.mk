# $Id$

CC = cc
AR = ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC -I ../VC6Compat
LDFLAGS =
LIBS =
EXEC =
LIBSUF = .so
