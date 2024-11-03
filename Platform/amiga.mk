# $Id$

PREFIX = :Tewi

CC = m68k-amigaos-gcc
AR = m68k-amigaos-ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -mcrt=nix13
LDFLAGS = -mcrt=nix13
LIBS = -lsocket -lpthread
MODULE =
EXEC =
LIBSUF = .so
