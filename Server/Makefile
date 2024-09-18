# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = version.o main.o config.o server.o http.o module.o strptime.o $(EXTOBJS) $(PREOBJS)

all: tewi$(EXEC) $(INSTALLER)

install.exe: tewi$(EXEC) install.nsi
	makensis install.nsi

tewi$(EXEC): $(OBJS) ../Common/common.a
	$(CC) $(LDFLAGS) $(EXTLDFLAGS) -o $@ $(OBJS) $(EXTLIBS) $(LIBS) ../Common/common.a

.c.o:
	$(CC) $(CFLAGS) $(EXTCFLAGS) -c -o $@ $<

tewi.res: tewi.rc tewi.ico
	$(WINDRES) tewi.rc -O coff -o $@

clean:
	rm -f *.o tewi *.exe
