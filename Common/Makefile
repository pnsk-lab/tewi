# $Id$

OBJ=o
STATIC=a
AR_FLAGS=rcs 
include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .$(OBJ)

OBJS = string.$(OBJ) log.$(OBJ) dir.$(OBJ)

all: common.$(STATIC)

common.$(STATIC): $(OBJS)
	$(AR) $(AR_FLAGS)$@ $(OBJS)

.c.$(OBJ):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a *.lib
