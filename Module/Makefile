# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o .so

all: mod_example.so

.o.so:
	$(CC) $(LDFLAGS) -shared -o $@ $< ../Common/common.a $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f *.o *.so *.a
