# $Id$

PWD = `pwd`
PLATFORM = generic
PREFIX = /usr/local

include Platform/$(PLATFORM).mk

FLAGS = PWD=$(PWD) PLATFORM=$(PLATFORM) PREFIX=$(PREFIX)

.PHONY: all format clean ./Server ./Common ./Module ./Manpage get-version

all: ./Server ./Module ./Manpage

./Server/option: ./Server/option.c
	cc -o $@ ./Server/option.c

./Server:: ./Common ./Server/option
	$(MAKE) -C $@ $(FLAGS) EXTOBJS="`./Server/option objs ../`" EXTLIBS="`./Server/option libs ../`" EXTCFLAGS="`./Server/option cflags ../`" EXTLDFLAGS="`./Server/option ldflags ../`"

./Module:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Common::
	$(MAKE) -C $@ $(FLAGS)

./Manpage::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i `find ./Server ./Common ./Module "(" -name "*.c" -or -name "*.h" ")" -and -not -name "strptime.*"` config.h

get-version:
	@grep "define TW_VERSION" Server/tw_version.h | grep -Eo '"[^\]+' | sed -E 's/^"//g'

clean:
	$(MAKE) -C ./Server $(FLAGS) clean
	$(MAKE) -C ./Module $(FLAGS) clean
	$(MAKE) -C ./Common $(FLAGS) clean
	$(MAKE) -C ./Manpage $(FLAGS) clean
	rm -f ./Server/option
