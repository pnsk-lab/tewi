# $Id$

OBJ=o
STATIC=a
AR_FLAGS=rcs
AR_PROC=cat
AR_USUAL=rcs
include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .$(OBJ)

OBJS = string.$(OBJ) log.$(OBJ) dir.$(OBJ)

all: common.$(STATIC)

common.a: $(OBJS)
	rm -f common.a
	$(AR) $(AR_USUAL) $@ `echo $(OBJS) | $(AR_PROC)`
	$(MAYBE_RANLIB)

common.lib: $(OBJS)
	rm -f common.lib
	$(AR) $(AR_FLAGS)$@ `echo $(OBJS) | $(AR_PROC)`

.c.$(OBJ):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a *.lib *.obj
