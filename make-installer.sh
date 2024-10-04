#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
	echo "Usage: $0 win32|win64|vc6|bcc [description]"
	exit 1
fi

fail() {
	rm -f tewi.exe
	rm -f generated.conf
	rm -f itworks.html
	exit 1
}

rm -f install.exe

VERSION=`make get-version`

make clean || fail
make PLATFORM=$1 PREFIX=C:/Tewi -j4 || fail
cp Server/tewi.exe tewi.exe
cd Server
../Tool/genconf "C:/Tewi" modules dll > ../generated.conf
../Tool/itworks > ../itworks.html
makensis -DVERSION=$VERSION -DONLY_EXEC install.nsi
cp install.exe ../install.exe
cd ..
rm tewi.exe
rm itworks.html
rm generated.conf
