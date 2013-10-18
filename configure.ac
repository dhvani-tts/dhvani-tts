#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_pattern_allow
AC_INIT([Dhvani],[0.95],[santhosh.thottingal@gmail.com],[dhvani])
AM_INIT_AUTOMAKE
USERCFLAGS=${CFLAGS}
AC_PROG_CC
AC_LANG(C++)
AM_PROG_CC_C_O
AC_CONFIG_MACRO_DIR([m4])

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

if test "x${DATABASEDIR}" = "x"; then
   DATABASEDIR=${datarootdir}/dhvani/database
fi
AC_ARG_VAR(DATABASEDIR,[directory contains voice files])
AC_CHECK_HEADER([gsm/gsm.h],, [AC_MSG_ERROR([[cannot find zlib header]])])
AC_CHECK_LIB([gsm], [gsm_decode],, [AC_MSG_ERROR([[cannot find gsm. Install libgsm package.]])])

PKG_CHECK_MODULES([GSTREAMER],
	[gstreamer-1.0 gstreamer-plugins-bad-1.0 gstreamer-plugins-base-1.0],
	[gstreamer_available="true"],
	[gstreamer_available="false"])
AC_ARG_WITH([gstreamer],
		AS_HELP_STRING(--with-gstreamer, Compile with GStreamer support),
		[
			if test ${withval} = "no"; then
			   gstreamersupport="false"
			elif test ${gstreamer_available} = "false"; then
			   gstreamersupport="false"
			   AC_MSG_ERROR([gstreamer libs not available])
			else
			   gstreamersupport="true"
			fi
		],
		[
			if test ${gstreamer_available} = "false"; then
			   gstreamersupport="false"
			else
			   gstreamersupport="true"
			fi
		])
AS_IF([test ${gstreamersupport} = "true"], [AC_MSG_NOTICE([Compiling with Gstreamer..])])
AM_CONDITIONAL([gstreamer_support], [test ${gstreamersupport} = "true"])

AC_CHECK_LIB(asound, snd_pcm_open, [alsa_available="true"], [alsa_available="false"])
AC_ARG_WITH(alsa,
		AS_HELP_STRING(--with-alsa, Compile with ALSA support),
		[
			if test ${withval} = "no"; then
			   alsasupport="false"
			elif test ${alsa_available} = "false"; then
			   alsasupport="false"
			   AC_MSG_ERROR([ALSA lib not available])
			else
			   alsasupport="true"
			fi
		],
		[
			if test ${gstreamersupport} = "true"; then
			   alsasupport="false"
			elif test ${alsa_available} = "true"; then
			   alsasupport="true"
			else
			   alsasupport="false"
			   AC_MSG_ERROR([ALSA and GStreamer both not available])
			fi
		])
AS_IF([test ${alsasupport} = "true"], [AC_MSG_NOTICE([Compiling with ALSA..])])
AM_CONDITIONAL(alsa_support, [test ${alsasupport} = "true"])

AC_CHECK_HEADER(soundtouch/SoundTouch.h,[soundtouch_available="true"], [soundtouch_available="false"])
AC_ARG_WITH(soundtouch,
		AS_HELP_STRING(--with-soundtouch, Compile with Soundtouch support),
		[
			if test ${withval} = "no"; then
			   soundtouchsupport="false"
			elif test ${soundtouch_available} = "false"; then
			   soundtouchsupport="false"
			   AC_MSG_ERROR([Soundtouch lib not available])
			else
			   soundtouchsupport="true"
			fi
		],
		[
			soundtouchsupport="false"
		])
AS_IF([test ${soundtouchsupport} = "true"], [AC_MSG_NOTICE([Compiling with Soundtouch..])])
AM_CONDITIONAL(soundtouch_support, [test ${soundtouchsupport} = "true"])

AC_CHECK_LIB(vorbisenc, vorbis_encode_init, [vorbis_available="true"], [vorbis_available="false"])
AC_ARG_WITH(vorbis,
		AS_HELP_STRING(--with-vorbis, Compile with Vorbis support),
		[
			if test ${withval} = "no"; then
			   vorbissupport="false"
			elif test ${vorbis_available} = "false"; then
			   vorbissupport="false"
			   AC_MSG_ERROR([Vorbis lib not available])
			else
			   vorbissupport="true"
			fi
		],
		[
			vorbissupport="false"
		])
AS_IF([test ${vorbissupport} = "true"], [AC_MSG_NOTICE([Compiling with Vorbis..])])
AM_CONDITIONAL(vorbis_support, [test ${vorbissupport} = "true"])

AC_ARG_ENABLE(debugging,
		AS_HELP_STRING([--enable-debugging@<:@=debugfilename@:>@], [enable debugging support]),
		[
			if test "${enableval}" = "no"; then
			   debugsupport="false"
			else
			   AC_SUBST([DEBUGFILENAME], $enableval)
			   debugsupport="true"
			fi
		 ],
		 [
			debugsupport="false"
		 ])
if test "x${USERCFLAGS}" = "x"; then
   CFLAGS="";
   AC_MSG_NOTICE([removed automake's default CFLAGS settings..]);
fi
AS_IF([test ${debugsupport} = "true"], [AC_MSG_NOTICE([Compiling with debugging enabled..])])
AM_CONDITIONAL([debug_support], [test ${debugsupport} = "true"])

AC_PATH_PROGS(BASH, bash)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([
Makefile
src/soundtouch4c/Makefile
src/Makefile
man/Makefile
database/Makefile
src/libdhvani.pc
])

AC_PROG_INSTALL
AC_OUTPUT
