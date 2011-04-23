#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_pattern_allow
AC_INIT([Dhvani],[0.95],[santhosh.thottingal@gmail.com],[dhvani])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_LANG(C++)
AM_PROG_CC_C_O

# Checks for programs.
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
dnl Enable largefile support
AC_SYS_LARGEFILE

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME


AC_CHECK_HEADER(soundtouch/SoundTouch.h,
	soundtouch="true";echo "Compiling with Soundtouch(Pitch and Tempo Control)  support.";,
 	soundtouch="false";echo "*** Soundtouch API library missing. Compiling without Soundtouch(Pitch and Tempo Control) support! See INSTALL.";)
 
AC_ARG_WITH(soundtouch, AS_HELP_STRING(--with-soundtouch, Compile with Soundtouch(Pitch and Tempo Control) support),
		 if test $withval = "no"; then 
		    echo "Forced compilation without Soundtouch support.";
		    soundtouch="false";
		 else 
		    if test $soundtouch = "false"; then
		       echo "Soundtouch support is not available. Sorry."; exit 1;
		    fi
                 fi
 		 echo "")

AM_CONDITIONAL(soundtouch_support, test $soundtouch = "true")

AC_CHECK_HEADER([gsm.h],, [AC_MSG_ERROR([[cannot find zlib header]])])
AC_CHECK_LIB([gsm], [gsm_decode],, [AC_MSG_ERROR([[cannot find gsm. Install libgsm package.]])])

AC_CHECK_LIB(vorbisenc, vorbis_encode_init, 
		oggvorbis="true";echo "Compiling with OggVorbis support.";,
        oggvorbis="false";echo "*** OggVorbis Library missing. Compiling without OggVorbis support! See INSTALL.";)

AC_ARG_WITH(oggvorbis, AS_HELP_STRING(--with-oggvorbis, Compile with OggVorbis support),
		 if test $withval = "no"; then 
		    echo "Forced compilation without OggVorbis support.";
		    oggvorbis="false";
		 else 
		    if test $oggvorbis = "false"; then
		       echo "OggVorbis support is not available. Sorry."; exit 1;
		    fi
                 fi
 		 echo "")

AM_CONDITIONAL(oggvorbis_support, test $oggvorbis = "true")

withdebug="false";
AC_ARG_WITH(debug, AS_HELP_STRING(--with-debug    Compile with debugging support.  default = no),
		 if test $withval = "yes"; then 
		    echo "Compiling with debug support.";
		    withdebug="true";
		 else
		       echo "Compiling without debug support";
         fi
 		 )
 		 
AM_CONDITIONAL(debug_support, test $withdebug = "true")


AC_CHECK_LIB(asound,
        snd_pcm_open,
		alsa_aud="true";echo "Compiling with ALSA support.";,
        alsa_aud="false";echo "*** ALSA C API library missing. Compiling without ALSA support! See INSTALL.";)

AC_ARG_WITH(alsa, AS_HELP_STRING(--with-alsa, Compile with ALSA support),
		 if test $withval = "no"; then 
		    echo "Forced compilation without ALSA support.";
		    alsa_aud="false";
		 else 
		    if test $alsa_aud = "false"; then
		       echo "ALSA support is not available. Sorry."; exit 1;
		    fi
                 fi
 		 echo "")

AM_CONDITIONAL(alsa_support, test $alsa_aud = "true")


AC_PATH_PROGS(BASH, bash)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([
Makefile
src/soundtouch4c/Makefile
src/Makefile
man/Makefile
database/Makefile
])

AC_PROG_INSTALL
AC_OUTPUT
