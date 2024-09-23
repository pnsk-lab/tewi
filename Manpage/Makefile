# $Id$

.PHONY: all clean

all: tewi.8

tewi.8: tewi.8.tmpl
	sed "s%@PREFIX@%$(PREFIX)%g" tewi.8.tmpl > $@ || rm -f $@

clean:
	rm -f tewi.8
