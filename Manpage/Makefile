# $Id$

.PHONY: all clean

all: tewi.conf.5

tewi.conf.5: tewi.conf.5.tmpl
	sed "s%@PREFIX@%$(PREFIX)%g" tewi.conf.5.tmpl > $@ || rm -f $@

clean:
	rm -f tewi.conf.5
