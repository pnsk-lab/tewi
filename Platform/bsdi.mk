# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../VC6Compat -I $(PWD)/Common -fPIC
LDFLAGS =
LIBS =
EXEC =
LIBSUF = .so
