#!/bin/sh
# $Id$

fail() {
	rm -f tewi-service.exe
	rm -f tewi.exe
	exit 1
}

VERSION=`make get-version`

make clean || fail
make PLATFORM=$1 -j4 || fail
cp Server/tewi.exe tewi.exe
make clean || fail
make PLATFORM=$1-service -j4 || fail
cp Server/tewi.exe tewi-service.exe
cd Server
makensis -DVERSION=$VERSION install.nsi
rm -f tewi.exe tewi-service.exe
cd ..
