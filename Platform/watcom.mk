# $Id$

PREFIX = C:/Tewi

CC = owcc
AR = wlib
AR_FLAGS = -q -b -n -fo
CFLAGS = -b nt$(END) -I $(WATCOM)/h/nt -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS = -b nt$(END)
LIBS = ws2_32.lib
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
