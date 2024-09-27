# $Id$

CC = psp-gcc
AR = psp-ar
CFLAGS = -g -std=c99 -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I /usr/local/pspdev/psp/sdk/include -D_PSP_FW_VERSION=600
LDFLAGS = -L /usr/local/pspdev/psp/sdk/lib
LIBS = -lpspgum -lpspgu -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspnet -lpspnet_apctl
EXEC = .elf
LIB = .so
MODULE =
SERVADD = psp-fixup-imports tewi.elf && psp-strip tewi.elf -o tewi_strip.elf
TARGET = tewi.pbp
