#!/bin/sh
# $Id$
# Wrapper for CL. VC6 sucks.

outfile="a.out"
dowhat=""
options="-I../VC6Compat -tWM"
obj=0
source=""
libraries=""
link=""
shared=0

for i in "$@"; do
	if [ "$i" = "-o" ]; then
		dowhat="output"
	elif [ "$i" = "-I" ]; then
		dowhat="include"
	elif [ "$i" = "-c" ]; then
		options="$options -c"
		obj=1
	elif [ "$i" = "-fPIC" ]; then
		:
	elif [ "$i" = "-g" ]; then
		:
	elif [ "$i" = "-std=c99" ]; then
		:
	elif [ "$i" = "-shared" ]; then
		options="$options -tWD"
		shared=1
	elif [ "`echo "$i" | grep -Eo "^-D"`" = "-D" ]; then
		options="$options -`echo "$i" | sed "s/^-//g"`"
	elif [ "`echo "$i" | grep -Eo "^-l"`" = "-l" ]; then
		libraries="$libraries `echo "$i" | sed "s/^-l//g"`.lib"
	elif [ "$dowhat" = "output" ]; then
		dowhat=""
		outfile="$i"
	elif [ "$dowhat" = "include" ]; then
		dowhat=""
		options="$options -I$i"
	elif [ ! "`echo "$i" | grep -Eo "^."`" = "-" ]; then
		source="$source $i"
	fi
done
if [ "$obj" = "1" ]; then
	options="$options -o$outfile"
else
	options="$options -e$outfile"
fi
if [ ! "$libraries" = "" ]; then
	link="$libraries"
fi
construct="bcc32 $options $source $link"
echo "Run: $construct"
$construct
