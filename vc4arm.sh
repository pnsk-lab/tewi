#!/bin/sh
# $Id$
# Wrapper for CL. VC6 sucks.

outfile="a.out"
dowhat=""
options="/I../WCECompat /I../VC6Compat /D_NTSDK /Ddouble=long /DNEED_DIRECT /DARM /D_ARM_ /D_M_ARM /DARMV4I /D_ARMV4I_ /DWINCE /DUNICODE"
obj=0
win=0
source=""
libraries=""
link=""

for i in "$@"; do
	if [ "$i" = "-o" ]; then
		dowhat="output"
	elif [ "$i" = "-I" ]; then
		dowhat="include"
	elif [ "$i" = "-c" ]; then
		options="$options /c"
		obj=1
	elif [ "$i" = "-fPIC" ]; then
		:
	elif [ "$i" = "-g" ]; then
		:
	elif [ "$i" = "-std=c99" ]; then
		:
	elif [ "$i" = "-shared" ]; then
		options="$options /LD"
	elif [ "$i" = "-mwindows" ]; then
		win=1
	elif [ "`echo "$i" | grep -Eo "^-D"`" = "-D" ]; then
		options="$options /`echo "$i" | sed "s/^-//g"`"
	elif [ "`echo "$i" | grep -Eo "^-l"`" = "-l" ]; then
		if [ "$i" = "-luser32" ]; then
			libraries="$libraries gdi32.lib"
		fi
		if [ "$i" = "-lws2_32" ]; then
			libraries="$libraries ws2.lib"
		else
			libraries="$libraries `echo "$i" | sed "s/^-l//g"`.lib"
		fi
	elif [ "$dowhat" = "output" ]; then
		dowhat=""
		outfile="$i"
	elif [ "$dowhat" = "include" ]; then
		dowhat=""
		options="$options /I$i"
	elif [ ! "`echo "$i" | grep -Eo "^."`" = "-" ]; then
		source="$source $i"
	fi
done
if [ "$obj" = "1" ]; then
	options="$options /Fo$outfile"
else
	options="$options /Fe$outfile"
fi
if [ ! "$libraries" = "" ]; then
	link="/link /subsystem:windowsce $libraries /libpath:../WCECompat wcecompat.lib"
else
	link="/link /subsystem:windowsce"
fi
if [ "$obj" = "0" ]; then
	if [ "$win" = "1" ]; then
		link="$link /SUBSYSTEM:windowsce"
	fi
fi
construct="clarm /nologo $options $source $link"
echo "Run: $construct"
$construct
