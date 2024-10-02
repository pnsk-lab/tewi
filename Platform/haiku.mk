# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC
LDFLAGS =
LIBS = -lnetwork
EXEC =
LIBSUF = .so
