# $Id$

CC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I $(PWD)/openssl/include
LDFLAGS = -L $(PWD)/openssl/lib
LIBS =
EXEC = .exe
