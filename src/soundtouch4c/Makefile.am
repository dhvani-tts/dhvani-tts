EXTRA_DIST =
SUBDIRS = 
AUTOMAKE_OPTS= gnu
MAKE=make
NAME=jrtp4c

if  soundtouch_support	
SOUNDTOUCH_FLAGS = -DWITH_SOUNDTOUCH  
AM_CFLAGS   = -fpic -shared -Wall -O3 -Isrc -I/usr/local/include/soundtouch -I/usr/include/soundtouch
else
AM_CFLAGS   = -fpic -shared -Wall -O3 -Isrc 
endif

AM_CC       = g++
AM_CPPFLAGS = $(AM_CFLAGS) $(SOUNDTOUCH_FLAGS)

lib_LTLIBRARIES            = libsoundtouch4c.la
libsoundtouch4c_la_SOURCES = src/soundtouch4c.cpp
libsoundtouch4c_la_CFLAGS  = $(AM_CFLAGS) 
libsoundtouch4c_la_LDFLAGS = -lSoundTouch -lstdc++

library_includedir      = $(prefix)/include
library_include_HEADERS = src/soundtouch4c.h
