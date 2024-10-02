# $Id$

PREFIX = /dev_hdd0/httpd

CC = mips64r5900el-ps2-elf-gcc
AR = mips64r5900el-ps2-elf-ar
CFLAGS = -O2 -D_EE -DPREFIX=\"$(PREFIX)\" -I $(PWD)/Common -I /usr/local/ps2dev/ps2sdk/ee/include -I /usr/local/ps2dev/ps2sdk/common/include
LDFLAGS = -T/usr/local/ps2dev/ps2sdk/ee/startup/linkfile -L /usr/local/ps2dev/ps2sdk/ee/lib -O2
LIBS = -ldebug -lsocket
EXEC = .elf
LIBSUF = .so
MODULE =
SERVADD = mips64r5900el-ps2-elf-strip tewi.elf -o tewi_strip.elf
