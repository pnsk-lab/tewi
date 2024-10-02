# $Id$

OBJ=o
STATIC=a
include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .$(OBJ)

OBJS = main.$(OBJ) version.$(OBJ) config.$(OBJ) server.$(OBJ) http.$(OBJ) module.$(OBJ) strptime.$(OBJ) font.$(OBJ) $(EXTOBJS) $(PREOBJS)

all: tewi$(EXEC) $(TARGET)

tewi_strip$(EXEC): tewi$(EXEC)

tewi$(EXEC): $(OBJS) ../Common/common.$(STATIC) $(REQOBJS)
	$(CC) $(LDFLAGS) $(EXTLDFLAGS) -o $@ $(OBJS) $(EXTLIBS) ../Common/common.$(STATIC) $(LIBS)
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
	mkdir -p pkg/USRDIR/etc
	mkdir -p pkg/USRDIR/www
	cp ../Binary/ps3.png pkg/ICON0.PNG
	../Tool/genconf /dev_hdd0/game/TEWI_00-0/USRDIR lib/tewi so > pkg/USRDIR/etc/tewi.conf
	cp ../Binary/pbtewi.gif pkg/USRDIR/www/
	convert '../Binary/pbtewi.gif[0]' pkg/USRDIR/pbtewi.png
	../Tool/itworks > pkg/USRDIR/www/index.html
	make_self_npdrm tewi_strip$(EXEC) pkg/USRDIR/EBOOT.BIN UP0001-TEWI_00-0000000000000000
	sfo.py --title "Tewi HTTPd" --appid "TEWI" -f /usr/local/ps3dev/bin/sfo.xml pkg/PARAM.SFO
	pkg.py --contentid UP0001-TEWI_00-0000000000000000 pkg/ $@
	rm -rf pkg
	package_finalize $@

.c.$(OBJ):
	$(CC) $(CFLAGS) $(EXTCFLAGS) -c -o $@ $<

tewi.res: tewi.rc ../Binary/tewi.ico
	$(WINDRES) tewi.rc -O coff -o $@

tewi_vc6.res: tewi.rc ../Binary/tewi.ico
	rc /fo$@ tewi.rc

tewi_bcc.res: tewi.rc ../Binary/tewi.ico
	brc32 -r -fo$@ tewi.rc

clean:
	rm -f *.o tewi *.exe *.res *.elf *.sfo *.pbp *.self *.pkg *.obj
