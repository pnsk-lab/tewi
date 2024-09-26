#!/bin/sh
# $Id$

export WINEPATH='C:\cygwin\bin'
cat config.h.tmpl | sed -E 's/#undef (NO_IPV6|NO_SSL|NO_GETADDRINFO)/#define \1/g' > config.h
wine bash cygwin.sh
