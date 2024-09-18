# $Id$

PREFIX = C:/Tewi

CC = i686-w64-mingw32-gcc
AR = i686-w64-mingw32-ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I $(PWD)/openssl/include -fPIC
LDFLAGS = -L $(PWD)/openssl/lib32
LIBS = -lws2_32
EXEC = .exe
LIB = .dll
