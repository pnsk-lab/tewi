# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = main.o version.o config.o server.o http.o module.o strptime.o font.o $(EXTOBJS) $(PREOBJS)

all: tewi$(EXEC) $(TARGET)

tewi_strip$(EXEC): tewi$(EXEC)

tewi$(EXEC): $(OBJS) ../Common/common.a
	$(CC) $(LDFLAGS) $(EXTLDFLAGS) -o $@ $(OBJS) $(EXTLIBS) ../Common/common.a $(LIBS)
	$(SERVADD)

tewi.pbp: tewi_strip$(EXEC) param.sfo
	pack-pbp $@ param.sfo ../Binary/psp.png NULL NULL NULL NULL tewi_strip$(EXEC) NULL

param.sfo:
	mksfoex -d MEMSIZE=1 'Tewi HTTPd' $@

tewi.self: tewi_strip$(EXEC)
	sprxlinker tewi_strip$(EXEC)
	make_self_npdrm tewi_strip$(EXEC) $@ UP0001-TEWI_00-0000000000000000
	fself tewi_strip$(EXEC) tewi.fake.self

tewi.pkg: tewi.self
	mkdir -p pkg/USRDIR
	cp ../Binary/ps3.png pkg/ICON0.PNG
	make_self_npdrm tewi_strip$(EXEC) pkg/USRDIR/EBOOT.BIN UP0001-TEWI_00-0000000000000000
	sfo.py --title "Tewi HTTPd" --appid "TEWI" -f /usr/local/ps3dev/bin/sfo.xml pkg/PARAM.SFO
	pkg.py --contentid UP0001-TEWI_00-0000000000000000 pkg/ $@
	rm -rf pkg
	package_finalize $@

.c.o:
	$(CC) $(CFLAGS) $(EXTCFLAGS) -c -o $@ $<

tewi.res: tewi.rc ../Binary/tewi.ico
	$(WINDRES) tewi.rc -O coff -o $@

clean:
	rm -f *.o tewi *.exe *.res *.elf *.sfo *.pbp *.self *.pkg
