# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = version.o main.o config.o

all: tewi$(EXEC)

tewi$(EXEC): $(OBJS) ../Common/common.a
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) ../Common/common.a -lssl -lcrypto

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o tewi *.exe
