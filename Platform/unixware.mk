# $Id$

CC = gcc
AR = ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -D_DEFAULT_SOURCE -fPIC -D_XOPEN_SOURCE=600
LDFLAGS =
LIBS =
EXEC =
LIBSUF = .so
