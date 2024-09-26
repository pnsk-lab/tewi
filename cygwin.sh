#!/bin/sh
# $Id$
make PREFIX=/cygdrive/c/Tewi DESTDIR=tewidist PLATFORM=cygwin install
cp /usr/bin/cygwin1.dll tewidist/
zip -rv tewidist.zip tewidist
