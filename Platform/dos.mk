# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = DOS_WATCOM
CC = owcc
AR = wlib
AR_FLAGS = -q -b -n -fo
CFLAGS = -b dos4g$(END) -I $(WATCOM)/h -I $(PWD)/watt32/inc -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS = -b dos4g$(END)
LIBS = $(PWD)/watt32/lib/wattcpwf.lib
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
MODULE=
