# $Id$

PREFIX = C:/Tewi

PLATFORM_IDENT = WINDOWS
CC = ../bcc.sh
AR = tlib
AR_FLAGS =
AR_PROC = grep -Eo "[^ ]+" | xargs -I {} echo + {}
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS =
LIBS =
EXEC = .exe
STATIC = lib
LIBSUF = .dll
OBJ = obj
REQOBJS = gui_bcc.res
