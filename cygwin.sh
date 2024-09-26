#!/bin/sh
# $Id$
rm -rf tewidist
make PREFIX=/cygdrive/c/Tewi DESTDIR=tewidist PLATFORM=cygwin install
cp -rfv tewidist/cygdrive/c/Tewi tewidist/Tewi
cp /usr/bin/cygwin1.dll tewidist/Tewi/bin/
rm -rf tewidist/cygdrive
echo "@echo off" > tewidist/install.bat
echo "echo This installer will attempt to overwrite files." >> tewidist/install.bat
echo "echo Make sure you make backups." >> tewidist/install.bat
echo "pause" >> tewidist/install.bat
echo "xcopy /e Tewi C:\\Tewi" >> tewidist/install.bat
echo "echo Tewi HTTPd has been installed." >> tewidist/install.bat
echo "pause" >> tewidist/install.bat
echo "exit" >> tewidist/install.bat
unix2dos tewidist/install.bat
rm -f tewidist.zip
zip -rv tewidist.zip tewidist
rm -rf tewidist
