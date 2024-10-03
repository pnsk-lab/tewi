# $Id$

PWD = `pwd`
PLATFORM = generic
PLATFORM_IDENT = NOT_WINDOWS
PREFIX = /usr/local
MODULE = ./Module
OBJ = o

include Platform/$(PLATFORM).mk

FLAGS = PWD=$(PWD) PLATFORM=$(PLATFORM) PREFIX=$(PREFIX)

.PHONY: all format clean ./Server ./Common ./Module get-version

all: ./Server $(MODULE)

./Tool/option: ./Tool/option.c config.h
	cc -o $@ ./Tool/option.c

./Tool/genconf: ./Tool/genconf.c config.h
	cc -o $@ ./Tool/genconf.c

./Tool/itworks: ./Tool/itworks.c config.h
	cc -o $@ ./Tool/itworks.c

./Server:: ./Common ./Tool/option ./Tool/genconf ./Tool/itworks
	$(MAKE) -C $@ $(FLAGS) EXTOBJS="`./Tool/option objs ../ $(PLATFORM_IDENT) $(OBJ)`" EXTLIBS="`./Tool/option libs ../ $(PLATFORM_IDENT) $(OBJ)`" EXTCFLAGS="`./Tool/option cflags ../ $(PLATFORM_IDENT) $(OBJ)`" EXTLDFLAGS="`./Tool/option ldflags ../ $(PLATFORM_IDENT) $(OBJ)`"

./Module:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Common::
	$(MAKE) -C $@ $(FLAGS)

./README: ./README.tmpl ./Server/tw_version.h
	sed "s/@VERSION@/`grep "define TW_VERSION" Server/tw_version.h | grep -Eom 1 '"[^\]+' | sed 's/^"//g'`/g" ./README.tmpl > $@

install: all ./Tool/genconf ./Tool/itworks
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib/tewi $(DESTDIR)$(PREFIX)/etc $(DESTDIR)$(PREFIX)/www
	if [ ! -e $(DESTDIR)$(PREFIX)/etc/tewi.conf ]; then ( ./Tool/genconf $(PREFIX) lib/tewi so > $(DESTDIR)$(PREFIX)/etc/tewi.conf || ( rm $(DESTDIR)$(PREFIX)/etc/tewi.conf ; exit 1 ) ) ; fi
	if [ ! -e $(DESTDIR)$(PREFIX)/www/index.html ]; then ( ./Tool/itworks > $(DESTDIR)$(PREFIX)/www/index.html || ( rm $(DESTDIR)$(PREFIX)/www/index.html ; exit 1 ) ) ; fi
	if [ ! -e $(DESTDIR)$(PREFIX)/www/pbtewi.gif ]; then ( cp Binary/pbtewi.gif $(DESTDIR)$(PREFIX)/www/ || ( rm $(DESTDIR)$(PREFIX)/www/pbtewi.gif ; exit 1 ) ) ; fi
	-cp ./Server/tewi $(DESTDIR)$(PREFIX)/bin/
	-cp ./Module/*.so $(DESTDIR)$(PREFIX)/lib/tewi/

format:
	clang-format --verbose -i `find ./Server ./Common ./Module ./Tool "(" -name "*.c" -or -name "*.h" ")" -and -not -name "strptime.*"` config.h

get-version:
	@grep "define TW_VERSION" Server/tw_version.h | grep -Eom 1 '"[^\]+' | sed 's/^"//g'

clean:
	$(MAKE) -C ./Server $(FLAGS) clean
	$(MAKE) -C ./Module $(FLAGS) clean
	$(MAKE) -C ./Common $(FLAGS) clean
	rm -f ./Tool/option ./Tool/genconf ./Tool/itworks
