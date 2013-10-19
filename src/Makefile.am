RELEASE = 1
bin_PROGRAMS = dhvani
lib_LTLIBRARIES = libdhvani.la
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)
GSM_LIB=-lgsm

if soundtouch_support	
SOUNDTOUCH_CFLAGS = -DWITH_SOUNDTOUCH  -Isoundtouch4c/src
SOUNDTOUCH_LIBS = -Lsoundtouch4c -lsoundtouch4c
SOUNDTOUCH_SOURCES = soundtouch_utils.h soundtouch_utils.c
endif

if alsa_support
ALSA_CFLAGS = -DWITH_ALSA
ALSA_LIBS = -lasound
ALSA_SOURCES = alsa_player.h alsa_player.c
endif

if vorbis_support
OGGVORBIS_FLAGS = -DWITH_OGGVORBIS
OGGVORBIS_LIBS = -lvorbisenc
OGGVORBIS_SOURCES = oggencoder.h oggencoder.c
endif

if debug_support
WITH_DEBUG = -g -O0 -Wall -DDEBUGFILENAME=\"@DEBUGFILENAME@\"
endif

if gstreamer_support
GSTREAMER_CFLAGS += -DWITH_GSTREAMER
GSTREAMER_SOURCES = gstplay.h gstplay.c
endif

AM_CFLAGS = $(ALSA_CFLAGS) $(OGGVORBIS_CFLAGS) $(SOUNDTOUCH_CFLAGS) $(GSTREAMER_CFLAGS) -DDATABASEPATH=$(DATABASEDIR) $(WITH_DEBUG)
libdhvani_includedir=$(includedir)/dhvani
libdhvani_include_HEADERS = dhvani_lib.h
libdhvani_la_LIBADD =  $(ALSA_LIBS) $(OGGVORBIS_LIBS)  $(SOUNDTOUCH_LIBS) $(GSTREAMER_LIBS) $(GSM_LIB)
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
	UTF8Decoder.c \
	UTF8Decoder.h \
	synthesizer.c \
	synthesizer.h \
	dhvani_utils.h \
	dhvani_utils.c \
	$(ALSA_SOURCES) $(SOUNDTOUCH_SOURCES) $(OGGVORBIS_SOURCES) $(GSTREAMER_SOURCES)
dhvani_SOURCES= dhvani.c 
dhvani_CFLAGS = $(SOUNDTOUCH_FLAGS) $(OGGVORBIS_FLAGS) -DDATABASEPATH=$(DATABASEDIR) $(WITH_DEBUG)
dhvani_LDADD= -ldhvani

pkgconfigdir= ${libdir}/pkgconfig
pkgconfig_DATA= libdhvani.pc
