# $Id$

OBJ=o
STATIC=a
END=_dll
include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .$(OBJ) $(LIBSUF)

all: mod_cgi$(LIBSUF) mod_proxy$(LIBSUF)

.$(OBJ)$(LIBSUF):
	$(CC) $(LDFLAGS) -shared -o $@ $< ../Common/common.$(STATIC) $(LIBS)

.c.$(OBJ):
	$(CC) $(CFLAGS) -I ../Server -c -o $@ $<

clean:
	rm -f *.o *.so *.a *.dll
