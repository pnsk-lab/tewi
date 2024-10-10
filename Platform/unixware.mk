# $Id$

CC = cc
AR = ar
CFLAGS = -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I $(PWD)/VC6Compat
LDFLAGS =
SHARED = -G
LIBS = -lnetwork -ldl
EXEC =
LIBSUF = .so
