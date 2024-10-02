# $Id$

PREFIX = C:/Tewi

CC = i686-w64-mingw32-gcc
AR = i686-w64-mingw32-ar
WINDRES = i686-w64-mingw32-windres
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC -DSERVICE -mwindows
LDFLAGS = -mwindows
LIBS = -lws2_32
EXEC = .exe
LIBSUF = .dll
PREOBJS = tewi.res
