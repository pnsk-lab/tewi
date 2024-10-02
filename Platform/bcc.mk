# $Id$

PREFIX = C:/Tewi

CC = ../bcc.sh
AR = tlib
AR_FLAGS =
AR_PROC = grep -Eo "[^ ]+" | xargs -I {} echo + {}
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I ../Common -fPIC
LDFLAGS =
LIBS = -lws2_32
EXEC =
STATIC = lib
LIBSUF = .dll
OBJ = obj
REQOBJS = tewi_bcc.res
