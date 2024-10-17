# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I ../VC6Compat -fPIC
LDFLAGS =
LIBS = -ldl
EXEC =
LIBSUF = .so
