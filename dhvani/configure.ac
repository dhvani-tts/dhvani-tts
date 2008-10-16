#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_pattern_allow

AC_PREREQ(2.57)
AC_INIT([Dhvani],[0.94],[santhosh.thottingal@gmail.com],[Dhvani])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_LANG(C++)


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC

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
CFLAGS="-O" 

PKG_CHECK_MODULES(ALSA, [alsa >= 1.0.15],  ,
    [AC_MSG_ERROR([You must have the alsa dev libraries to build dhvani. Install libasound-dev package using your package manager.]) ] 
)
AC_CHECK_HEADER([soundtouch/SoundTouch.h],[
	AC_CHECK_LIB([SoundTouch],[soundtouch_ac_test],[
		AC_DEFINE(HAVE_LIBSOUNDTOUCH,1,"Define if we need to build dhvani with pitch and tempo features.")
 	],[
		AC_DEFINE(HAVE_LIBSOUNDTOUCH,0,"Define if we need to build dhvani with pitch and tempo features.")
	])
],[
AC_ARG_ENABLE(soundtouch,[  --enable-soundtouch    Compile with  Soundtouch support. default = yes], [
	if test "${enableval}" = yes; then
		AC_MSG_ERROR([To build dhvani with pitch and tempo features you need libsoundtouch-dev package.  Either install it using your package manager or use --enable-soundtouch=no argument with configure command.]) 
	else 
		AC_MSG_NOTICE([Building without Soundtouch Support])
		AC_DEFINE(HAVE_LIBSOUNDTOUCH,0,"Define if we need to build dhvani with pitch and tempo features.")
	fi
])
])

PKG_CHECK_MODULES(VORBIS, [vorbisenc >= 1.2.0],[
		AC_DEFINE(HAVE_VORBISENC,1,"Define if we need to build dhvani with ogg vorbis encoding support.")
 	],[
	AC_ARG_ENABLE(oggoutput,[  --enable-oggoutput    Compile with oggvorbis encoding support. default = yes ], [
	if test "${enableval}" = yes; then
		AC_MSG_ERROR([To build dhvani with ogg encoded output files, you need vorbisenc-dev package.  Either install it using your package manager or use --enable-oggoutput=no argument with configure command.]) 
		AC_DEFINE(HAVE_VORBISENC,0,"Define if we need to build dhvani with ogg vorbis encoding support.")
	else 
		AC_MSG_NOTICE([Building with oggvorbis Support])
		AC_DEFINE(HAVE_VORBISENC,1,"Define if we need to build dhvani with ogg vorbis encoding support.")
	fi
])
])

AC_ARG_WITH(debug,[  --with-debug    Compile with debugging support.  default = no], [
     CFLAGS="-g -O"
     AC_DEFINE(DHVANI_DEBUG_ENABLED,1,"Define if you need debug messages")
])

 
AC_PATH_PROGS(BASH, bash)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([
Makefile
src/soundtouch4c/Makefile
man/Makefile
])

AC_PROG_INSTALL
AC_OUTPUT
