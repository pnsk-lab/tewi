# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = WINDOWS_WATCOM
CC = owcc
AR = wlib
AR_FLAGS = -q -b -n -fo
CFLAGS = -b nt$(END) -I $(WATCOM)/h/nt -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS = -b nt$(END)
LIBS =
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
SERVADD = wrc -bt=nt -i=$(WATCOM)/h/nt -fe=tewi.exe tewi.rc
