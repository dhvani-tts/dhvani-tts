
BINDIR=$(PREFIX)/usr/bin
INCDIR=$(PREFIX)/usr/include/dhvani
LIBDIR=$(PREFIX)/usr/lib
DATADIR=$(PREFIX)/usr/share/dhvani

SOUNDTOUCHSRC=soundtouch4c/src
RELEASE = 1
BIN_NAME = dhvani
LIB_NAME = libdhvani.so
STATIC_LIB_NAME = libdhvani.a
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)
 
EXTRA_LIBS =  -lasound -lvorbisenc -lsoundtouch4c -lm ../gsm/lib/*.o
SOURCE_LIBS = -L. libdhvani.so

INSTALL = install
LN_SF = /bin/ln -sf
MKDIR = mkdir -p

dhvani_SOURCES =	dhvani.c  
	
libdhvani_SOURCES =debug.c \
	debug.h \
	dhvani_lib.c \
	dhvani_lib.h \
	languages.c \
	languages.h \
	phonetic_synthesizer_ml.c  \
	phonetic_synthesizer_ml.h  \
	phonetic_synthesizer_ka.c  \
	phonetic_synthesizer_ka.h  \
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


SRCS1=$(dhvani_SOURCES)
OBJS1=$(patsubst %.c,%.o,$(SRCS1))
LIBS1=$(EXTRA_LIBS)

SRCS2=$(libdhvani_SOURCES)
OBJS2=$(patsubst %.c,%.o,$(SRCS2))
LIBS2=$(SOURCE_LIBS) $(EXTRA_LIBS)
 
 
CXXFLAGS=-O1 -g


all: $(LIB_NAME) $(BIN_NAME)  $(STATIC_LIB_NAME) 
	mv $(LIB_NAME) $(LIB_NAME).$(LIBTAG)

.c.o:
	gcc $(CXXFLAGS)  -pedantic -I. -I$(SOUNDTOUCHSRC) -c -fno-exceptions  $<

$(BIN_NAME): $(OBJS1)
	gcc -o $@ -I$(SOUNDTOUCHSRC)  $(OBJS1) $(LIBS2)

%.o: %.c
	gcc $(CXXFLAGS)  -fpic -pedantic \
	-I. -I$(SOUNDTOUCHSRC) -D LIBRARY -c -fno-exceptions  $<  -o $*.o

$(LIB_NAME): $(OBJS2)
	gcc -shared  -I$(SOUNDTOUCHSRC) -o $@ $(OBJS2) $(LIBS1) 

$(STATIC_LIB_NAME): $(OBJS2)
	$(AR) cqs $(STATIC_LIB_NAME) $(OBJS2)

clean:
	rm -f *.o *.a *~  *.so.* *.so $(BIN_NAME)

distclean: clean
	rm -f $(BIN_NAME)
	rm -f $(BIN2_NAME)
	rm -f $(LIB_NAME)*

install: all
	# Create directories
	rm -rf $(DESTDIR)$(DATADIR)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	# Install dhvani executable
	$(INSTALL) -m 755 $(BIN_NAME) $(DESTDIR)$(BINDIR)
	# Install shared library
	$(INSTALL) -m 755 $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)
	# Install static library
	$(INSTALL) -m 755 $(STATIC_LIB_NAME) $(DESTDIR)$(LIBDIR)
	ranlib $(DESTDIR)$(LIBDIR)/$(STATIC_LIB_NAME)
	$(LN_SF) $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIB_NAME).$(LIB_VERSION)
	$(LN_SF) $(LIB_NAME).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIB_NAME)
	# Install development headers
	$(INSTALL) -pm 644 dhvani_lib.h $(DESTDIR)$(INCDIR)
	ldconfig -n $(DESTDIR)$(LIBDIR)
	# Install data files
	cp -pRrf ../database $(DESTDIR)$(DATADIR)

