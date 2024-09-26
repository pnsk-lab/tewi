# $Id$

CC = cc
AR = ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -fPIC -DNO_IPV6
LDFLAGS =
LIBS =
EXEC =
LIB = .so
