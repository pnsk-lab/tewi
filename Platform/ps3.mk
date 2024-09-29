# $Id$

PREFIX = /dev_hdd0/httpd

CC = ppu-gcc
AR = ppu-ar
CFLAGS = -g -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I /usr/local/ps3dev/ppu/include
LDFLAGS = -L /usr/local/ps3dev/ppu/lib
LIBS = -lnet -lsysmodule -lsysutil -lrt -llv2 -lrsx -lgcm_sys
EXEC = .elf
LIB = .so
MODULE =
SERVADD = ppu-strip tewi.elf -o tewi_strip.elf
TARGET = tewi.pkg
