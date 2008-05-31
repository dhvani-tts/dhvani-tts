# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

PREFIX=/
BINDIR=$(PREFIX)/usr/local/bin
INCDIR=$(PREFIX)/usr/include/dhvani
LIBDIR=$(PREFIX)/usr/lib
DATADIR=$(PREFIX)/usr/share/dhvani

RELEASE = 1
BIN_NAME = dhvani
LIB_NAME = libdhvani.so
STATIC_LIB_NAME = libdhvani.a
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)
 
EXTRA_LIBS =  -lasound -lm ../gsm/lib/*.o

INSTALL = install
LN_SF = /bin/ln -sf
MKDIR = mkdir -p

dhvani_SOURCES = dhvani.c \
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
	synthesizer.c \
	synthesizer.h 

libdhvani_SOURCES =dhvani_lib.c \
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
	synthesizer.c \
	synthesizer.h 

SRCS1=$(dhvani_SOURCES)
OBJS1=$(patsubst %.c,%.o,$(SRCS1))
LIBS1=$(EXTRA_LIBS)

SRCS2=$(libdhvani_SOURCES)
OBJS2=$(patsubst %.c,x_%.o,$(SRCS2))
LIBS2=$(EXTRA_LIBS)
 
 
CXXFLAGS=-O1


all: $(BIN_NAME) $(LIB_NAME) $(STATIC_LIB_NAME) $(BIN2_NAME)
	mv $(LIB_NAME) $(LIB_NAME).$(LIBTAG)

.c.o:
	gcc $(CXXFLAGS)  -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -pedantic -I. -c -fno-exceptions  $<

$(BIN_NAME): $(OBJS1)
	gcc -o $@ $(OBJS1) $(LIBS1)

#$(BIN2_NAME): $(OBJS2) $(LIB_NAME)
#	gcc -o $@ $(OBJS2) $(LIBS2)



x_%.o: %.c
	gcc $(CXXFLAGS) $(USE_AUDIO) -Wall -g -fpic -pedantic \
	-I. -D LIBRARY -c -fno-exceptions  $<  -o x_$*.o

$(LIB_NAME): $(OBJS2)
	gcc -shared  -o $@ $(OBJS2) $(LIBS2)

$(STATIC_LIB_NAME): $(OBJS2)
	$(AR) cqs $(STATIC_LIB_NAME) $(OBJS2)

clean:
	rm -f *.o *.a *~  *.so.*

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
	$(LN_SF) $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIB_NAME).$(LIB_VERSION)
	$(LN_SF) $(LIB_NAME).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIB_NAME)
	# Install development headers
	$(INSTALL) -pm 644 dhvani_lib.h $(DESTDIR)$(INCDIR)
	# Install data files
	cp -pRrf ../database $(DESTDIR)$(DATADIR)

