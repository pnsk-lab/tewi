# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -D_DEFAULT_SOURCE -fPIC
LDFLAGS =
LIBS =
EXEC =
LIB = .so
