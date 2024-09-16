# $Id$

PWD = `pwd`
PLATFORM = generic
PREFIX = /usr/local

include Platform/$(PLATFORM).mk

FLAGS = PWD=$(PWD) PLATFORM=$(PLATFORM) PREFIX=$(PREFIX)

.PHONY: all format clean ./Server ./Common ./Module

all: ./Server ./Module

./Server:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Module:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Common::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i `find ./Server ./Common ./Module "(" -name "*.c" -or -name "*.h" ")" -and -not -name "strptime.*"`

clean:
	$(MAKE) -C ./Server $(FLAGS) clean
	$(MAKE) -C ./Module $(FLAGS) clean
	$(MAKE) -C ./Common $(FLAGS) clean
