# $Id$

CC = ../vc6.sh
AR = lib
AR_FLAGS = /out:
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS =
LIBS = -lws2_32 -ladvapi32 -llibcmt
EXEC =
STATIC = lib
LIBSUF = .dll
OBJ = obj
