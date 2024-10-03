# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = WINDOWS
CC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
WINDRES = x86_64-w64-mingw32-windres
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC -DSERVICE -mwindows
LDFLAGS = -mwindows
LIBS =
EXEC = .exe
LIBSUF = .dll
PREOBJS = tewi.res
