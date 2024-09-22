# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o $(LIB)

all: mod_cgi$(LIB) mod_proxy$(LIB)

.o$(LIB):
	$(CC) $(LDFLAGS) -shared -o $@ $< ../Common/common.a $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.a *.dll
