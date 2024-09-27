# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = version.o main.o config.o server.o http.o module.o strptime.o $(EXTOBJS) $(PREOBJS)

all: tewi$(EXEC) $(TARGET)

tewi_strip$(EXEC): tewi$(EXEC)

tewi$(EXEC): $(OBJS) ../Common/common.a
	$(CC) $(LDFLAGS) $(EXTLDFLAGS) -o $@ $(OBJS) $(EXTLIBS) $(LIBS) ../Common/common.a
	$(SERVADD)

tewi.pbp: tewi_strip$(EXEC) param.sfo
	pack-pbp $@ param.sfo NULL NULL NULL NULL NULL tewi_strip$(EXEC) NULL

param.sfo:
	mksfoex -d MEMSIZE=1 'Tewi HTTPd' $@

.c.o:
	$(CC) $(CFLAGS) $(EXTCFLAGS) -c -o $@ $<

tewi.res: tewi.rc ../Binary/tewi.ico
	$(WINDRES) tewi.rc -O coff -o $@

clean:
	rm -f *.o tewi *.exe *.res *.elf *.sfo *.pbp
