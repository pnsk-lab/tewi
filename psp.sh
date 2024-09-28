#!/bin/sh
# $Id$
export PSPDEV=/usr/local/pspdev
export PATH=$PATH:/usr/local/pspdev/bin
cat config.h.tmpl | sed -E 's/#undef (NO_SSL)/#define \1/g' > config.h
rm -rf httpd
make PLATFORM=psp DESTDIR=httpd/ install
mv httpd/ms0:/PSP/GAME/httpd/* httpd/
rm -rf httpd/lib httpd/bin
rm -rf httpd/ms0:
cp Server/tewi.pbp httpd/EBOOT.PBP
echo "Tewi HTTPd $(make get-version) for PSP" > httpd/README
echo "========================" >> httpd/README
echo "To install, just copy this \`httpd' folder into your /PSP/GAME of the PSP memorystick." >> httpd/README
cat httpd/readme
rm -f tewidist.zip
zip -rv tewidist.zip httpd
rm -rf httpd
