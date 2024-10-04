# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I /usr/local/include -fPIC
LDFLAGS = -Wl,-R/usr/local/lib/eopenssl33 -L/usr/local/lib/eopenssl33
LIBS =
EXEC =
LIBSUF = .so
