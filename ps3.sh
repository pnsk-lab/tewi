#!/bin/sh
# $Id$
export PS3DEV="/usr/local/ps3dev"
export PSL1GHT=$PS3DEV
export PATH=$PATH:${PS3DEV}/bin:${PS3DEV}/ppu/bin:${PS3DEV}/spu/bin
cat config.h.tmpl | sed -E 's/#undef (NO_SSL)/#define \1/g' > config.h
rm -rf TEWI_00-0
make PLATFORM=ps3 DESTDIR=TEWI_00-0/ install || exit 1
mkdir -p TEWI_00-0/USRDIR
mv TEWI_00-0/dev_hdd0/game/TEWI_00-0/USRDIR/* TEWI_00-0/USRDIR/
rm -rf TEWI_00-0/USRDIR/lib TEWI_00-0/USRDIR/bin
rm -rf TEWI_00-0/dev_hdd0
make_self_npdrm Server/tewi_strip.elf TEWI_00-0/USRDIR/EBOOT.BIN UP0001-TEWI_00-0000000000000000
sfo.py --title "Tewi HTTPd" --appid "TEWI" -f /usr/local/ps3dev/bin/sfo.xml TEWI_00-0/PARAM.SFO
cp Binary/ps3.png TEWI_00-0/ICON0.PNG
echo "Tewi HTTPd $(make get-version) for PS3" > TEWI_00-0/README
echo "========================" >> TEWI_00-0/README
echo "To install, just copy this \`TEWI_00-0' folder into your /game of PS3 HDD0." >> TEWI_00-0/README
cat TEWI_00-0/README
rm -f tewidist.zip
zip -rv tewidist.zip TEWI_00-0
rm -rf TEWI_00-0
