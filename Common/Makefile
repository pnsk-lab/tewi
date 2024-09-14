# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = string.o log.o dir.o

all: common.a

common.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a
