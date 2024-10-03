# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = WINDOWS
CC = ../vc6.sh
AR = lib
AR_FLAGS = /nologo /out:
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS =
LIBS = -ladvapi32 -llibcmt
EXEC =
STATIC = lib
LIBSUF = .dll
OBJ = obj
PREOBJS = tewi_vc6.res
