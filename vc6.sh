#!/bin/sh
# $Id$
# Wrapper for CL. VC6 sucks.

outfile="a.out"
dowhat=""
options="/I../VC6Compat"
obj=0
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
	elif [ "`echo "$i" | grep -Eo "^-D"`" = "-D" ]; then
		options="$options /`echo "$i" | sed "s/^-//g"`"
	elif [ "`echo "$i" | grep -Eo "^-l"`" = "-l" ]; then
		libraries="$libraries `echo "$i" | sed "s/^-l//g"`.lib"
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
	link="/link /nodefaultlib:libc $libraries"
fi
construct="cl /nologo $options $source $link"
echo "Run: $construct"
$construct
