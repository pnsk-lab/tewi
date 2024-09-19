#!/bin/sh
# $Id$

fail() {
	rm -f tewi-service.exe
	rm -f tewi.exe
	mv config.h.bak config.h
	exit 1
}

VERSION=`make get-version`

sed "s/undef NO_SSL/define NO_SSL/g" config.h.tmpl > config.h

make clean || fail
make PLATFORM=$1 -j4 || fail
cp Server/tewi.exe tewi.exe
make clean || fail
make PLATFORM=$1-service -j4 || fail
cp Server/tewi.exe tewi-service.exe
cd Server
makensis -DVERSION=$VERSION install.nsi
cp install.exe ../install-nossl.exe
rm -f tewi.exe tewi-service.exe
cd ..

sed "s/define NO_SSL/undef NO_SSL/g" config.h.tmpl > config.h

make clean || fail
make PLATFORM=$1 -j4 || fail
cp Server/tewi.exe tewi.exe
make clean || fail
make PLATFORM=$1-service -j4 || fail
cp Server/tewi.exe tewi-service.exe
cd Server
makensis -DVERSION=$VERSION install.nsi
cp install.exe ../install-ssl.exe
rm -f tewi.exe tewi-service.exe
cd ..
