# $Id$

PREFIX = C:/Tewi

CC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC
LDFLAGS =
LIBS = -lws2_32
EXEC = .exe
LIB = .dll
