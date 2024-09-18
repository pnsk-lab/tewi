# $Id$

PREFIX = C:/Tewi

CC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
WINDRES = x86_64-w64-mingw32-windres
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC
LDFLAGS =
LIBS = -lws2_32
EXEC = .exe
LIB = .dll
PREOBJS = tewi.res
INSTALLER = install.exe
