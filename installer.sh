#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
	echo "Usage: $0 win32"
	echo "       $0 win64"
	exit 1
fi

fail() {
	rm -f tewi-service.exe
	rm -f tewi.exe
	rm -f generated.conf
	rm -f itworks.html
	exit 1
}

rm -f install-nossl.exe install-ssl.exe

VERSION=`make get-version`

sed "s/undef NO_SSL/define NO_SSL/g" config.h.tmpl > config.h

make clean || fail
make PLATFORM=$1 -j4 || fail
cp Server/tewi.exe tewi.exe
make clean || fail
make PLATFORM=$1-service -j4 || fail
cp Server/tewi.exe tewi-service.exe
cd Server
../Tool/genconf "C:/Tewi" modules dll > ../generated.conf
../Tool/itworks > ../itworks.html
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
../Tool/genconf "C:/Tewi" modules dll > ../generated.conf
../Tool/itworks > ../itworks.html
makensis -DVERSION=$VERSION install.nsi
cp install.exe ../install-ssl.exe
rm -f tewi.exe tewi-service.exe
cd ..

rm itworks.html
rm generated.conf
