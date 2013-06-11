ACLOCAL_AMFLAGS = -I m4
am_cflags  = -fpic -shared -O2 -I/usr/include/ 
RELEASE = 1
bin_PROGRAMS = dhvani
lib_LTLIBRARIES = libdhvani.la
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)
DATABASEDIR = $(datarootdir)/dhvani/database/

GSM_LIB=-lgsm

if  soundtouch_support	
SOUNDTOUCH_FLAGS = -DWITH_SOUNDTOUCH  -Isoundtouch4c/src
SOUNDTOUCH_LIBS = -Lsoundtouch4c -lsoundtouch4c
endif

if alsa_support
ALSA_FLAGS = -DWITH_ALSA
ALSA_LIBS = -lasound
endif

if oggvorbis_support
OGGVORBIS_FLAGS = -DWITH_OGGVORBIS
OGGVORBIS_LIBS = -lvorbisenc
endif

if debug_support
WITH_DEBUG= -DWITH_DEBUG -g -Wall
endif

AM_CFLAGS = $(am_cflags) $(ALSA_FLAGS)  $(OGGVORBIS_FLAGS)  $(SOUNDTOUCH_FLAGS) $(WITH_DEBUG) -DDATABASEPATH=$(DATABASEDIR)
libdhvani_includedir=$(includedir)/dhvani
libdhvani_include_HEADERS = dhvani_lib.h
libdhvani_la_LIBADD =  $(ALSA_LIBS) $(OGGVORBIS_LIBS)  $(SOUNDTOUCH_LIBS)  $(GSM_LIB)
libdhvani_la_SOURCES = debug.c \
	debug.h \
	dhvani_lib.c \
	dhvani_lib.h \
	languages.c \
	languages.h \
	phonetic_synthesizer_ml.c  \
	phonetic_synthesizer_ml.h  \
	phonetic_synthesizer_kn.c  \
	phonetic_synthesizer_kn.h  \
	phonetic_synthesizer_hi.c  \
	phonetic_synthesizer_hi.h  \
	phonetic_synthesizer_gu.c  \
	phonetic_synthesizer_gu.h  \
	phonetic_synthesizer_or.c  \
	phonetic_synthesizer_or.h  \
	phonetic_synthesizer_te.c  \
	phonetic_synthesizer_te.h  \
	phonetic_synthesizer_pa.c  \
	phonetic_synthesizer_pa.h  \
	phonetic_synthesizer_bn.c  \
	phonetic_synthesizer_bn.h  \
	phonetic_synthesizer_ta.c  \
	phonetic_synthesizer_ta.h  \
	phonetic_synthesizer_mr.c  \
	phonetic_synthesizer_mr.h  \
	phonetic_synthesizer_ps.c  \
	phonetic_synthesizer_ps.h  \
	alsa_player.c \
	alsa_player.h \
	oggencoder.c \
	oggencoder.h \
	soundtouch_utils.c \
	soundtouch_utils.h \
	UTF8Decoder.c \
	UTF8Decoder.h \
	synthesizer.c \
	synthesizer.h 
dhvani_SOURCES= dhvani.c 
dhvani_CFLAGS = -Wall -g -O2  $(SOUNDTOUCH_FLAGS) $(OGGVORBIS_FLAGS) -DDATABASEPATH=$(DATABASEDIR)
dhvani_LDADD= -ldhvani 	
 
pkgconfigdir= ${libdir}/pkgconfig
pkgconfig_DATA= libdhvani.pc
