# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = WINDOWS
CC = ../vc4mips.sh
AR = lib
AR_FLAGS = /nologo /out:
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS =
LIBS = -ladvapi32 -llibcmt
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
PREOBJS = vc6.res