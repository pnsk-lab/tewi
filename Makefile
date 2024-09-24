# $Id$

PWD = `pwd`
PLATFORM = generic
PREFIX = /usr/local

include Platform/$(PLATFORM).mk

FLAGS = PWD=$(PWD) PLATFORM=$(PLATFORM) PREFIX=$(PREFIX)

.PHONY: all format clean ./Server ./Common ./Module ./Manpage get-version

all: ./Server ./Module ./Manpage ./Tool/genconf ./Tool/itworks

./Tool/option: ./Tool/option.c config.h
	cc -o $@ ./Tool/option.c

./Tool/genconf: ./Tool/genconf.c config.h
	cc -o $@ ./Tool/genconf.c

./Tool/itworks: ./Tool/itworks.c config.h
	cc -o $@ ./Tool/itworks.c

./Server:: ./Common ./Tool/option
	$(MAKE) -C $@ $(FLAGS) EXTOBJS="`./Tool/option objs ../`" EXTLIBS="`./Tool/option libs ../`" EXTCFLAGS="`./Tool/option cflags ../`" EXTLDFLAGS="`./Tool/option ldflags ../`"

./Module:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Common::
	$(MAKE) -C $@ $(FLAGS)

./Manpage::
	$(MAKE) -C $@ $(FLAGS)

install: all ./Tool/genconf ./Tool/itworks
	mkdir -p $(PREFIX)/bin $(PREFIX)/lib/tewi $(PREFIX)/share/man/man5 $(PREFIX)/etc $(PREFIX)/www
	if [ ! -e $(PREFIX)/etc/tewi.conf ]; then ( ./Tool/genconf $(PREFIX) lib/tewi so > $(PREFIX)/etc/tewi.conf || ( rm $(PREFIX)/etc/tewi.conf ; exit 1 ) ) ; fi
	if [ ! -e $(PREFIX)/www/index.html ]; then ( ./Tool/itworks > $(PREFIX)/www/index.html || ( rm $(PREFIX)/www/index.html ; exit 1 ) ) ; fi
	if [ ! -e $(PREFIX)/www/pbtewi.gif ]; then ( cp Binary/pbtewi.gif $(PREFIX)/www/ || ( rm $(PREFIX)/www/pbtewi.gif ; exit 1 ) ) ; fi
	cp ./Server/tewi $(PREFIX)/bin/
	cp ./Module/*.so $(PREFIX)/lib/tewi/
	cp ./Manpage/tewi.8 $(PREFIX)/share/man/man8/

format:
	clang-format --verbose -i `find ./Server ./Common ./Module ./Tool "(" -name "*.c" -or -name "*.h" ")" -and -not -name "strptime.*"` config.h

get-version:
	@grep "define TW_VERSION" Server/tw_version.h | grep -Eo '"[^\]+' | sed -E 's/^"//g'

clean:
	$(MAKE) -C ./Server $(FLAGS) clean
	$(MAKE) -C ./Module $(FLAGS) clean
	$(MAKE) -C ./Common $(FLAGS) clean
	$(MAKE) -C ./Manpage $(FLAGS) clean
	rm -f ./Tool/option ./Tool/genconf
