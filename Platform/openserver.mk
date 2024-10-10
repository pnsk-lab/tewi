# $Id$

SHELL = /usr/bin/bash

CC = gcc
AR = ar
CFLAGS = -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I $(PWD)/VC6Compat
LDFLAGS =
SHARED = -G
LIBS = -lsocket -ldl
EXEC =
LIBSUF = .so
