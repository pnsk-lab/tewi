# $Id$

PWD = `pwd`
PLATFORM = generic
PREFIX = /usr/local

FLAGS = PWD=$(PWD) PLATFORM=$(PLATFORM) PREFIX=$(PREFIX)

.PHONY: all format clean ./Server ./Common

all: ./Server

./Server:: ./Common
	$(MAKE) -C $@ $(FLAGS)

./Common::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i `find ./Server ./Common -name "*.c" -or -name "*.h"`

clean:
	$(MAKE) -C ./Server $(FLAGS) clean
	$(MAKE) -C ./Common $(FLAGS) clean
