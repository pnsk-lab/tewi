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

rm -rf Tewi
rm -f tewi.exe
rm -f itworks.html
rm -f generated.conf
rm -f install.exe
rm -f tewi.7z

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
rm -rf Tewi tewi.7z
7z x -oTewi install.exe
rm -rf Tewi/'$'*
7z a tewi.7z Tewi
rm -rf Tewi
rm tewi.exe
rm itworks.html
rm generated.conf
