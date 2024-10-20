#!/bin/sh
# $Id$
# Wrapper for BCC32.

outfile="a.out"
dowhat=""
options="-I../VC6Compat -tWM -I$BORLAND/Include -L$BORLAND/Lib"
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
	elif [ "$i" = "-mwindows" ]; then
		options="$options -tW"
	elif [ "`echo "$i" | grep -Eo "^-D"`" = "-D" ]; then
		options="$options -`echo "$i" | sed "s/^-//g"`"
	elif [ "`echo "$i" | grep -Eo "^-l"`" = "-l" ]; then
		if [ ! "$i" = "-lwsock32" -a ! "$i" = "-luser32" -a ! "$i" = "-lcomctl32" ]; then
			libraries="$libraries `echo "$i" | sed "s/^-l//g"`.lib"
		fi
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
