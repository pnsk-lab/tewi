# $Id$

OBJ=o
STATIC=a
END=_dll
SHARED=-shared
include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .$(OBJ) $(LIBSUF)

all: mod_cgi$(LIBSUF) mod_proxy$(LIBSUF)

.$(OBJ)$(LIBSUF):
	$(CC) $(LDFLAGS) $(SHARED) -o $@ $< $(LIBEXTRA) ../Common/common.$(STATIC) $(LIBS)

.c.$(OBJ):
	$(CC) $(CFLAGS) -I ../Server -c -o $@ $<

clean:
	rm -f *.o *.so *.a *.dll *.tds *.obj
