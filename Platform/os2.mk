# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = OS2_WATCOM
CC = owcc
AR = wlib
AR_FLAGS = -q -b -n -fo
CFLAGS = -b os2v2$(END) -I $(WATCOM)/h/os2 -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS = -b os2v2$(END)
LIBS =
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
REQOBJS = concat.rc
SERVADD = wrc -bt=nt -i=$(WATCOM)/h/nt concat.rc tewi.exe
