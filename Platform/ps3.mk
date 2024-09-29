# $Id$

PREFIX = /dev_hdd0/game/TEWI_00-0/USRDIR

CC = ppu-gcc
AR = ppu-ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I /usr/local/ps3dev/ppu/include -I /usr/local/ps3dev/portlibs/ppu/include
LDFLAGS = -L /usr/local/ps3dev/ppu/lib -L /usr/local/ps3dev/portlibs/ppu/lib 
LIBS = -lnet -lsysmodule -lsysutil -lrt -llv2 -lrsx -lgcm_sys -lpng -lm -lz
EXEC = .elf
LIB = .so
MODULE =
SERVADD = ppu-strip tewi.elf -o tewi_strip.elf
TARGET = tewi.pkg
