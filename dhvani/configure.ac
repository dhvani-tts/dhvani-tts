AC_INIT([Dhvani],[0.8],[santhosh],[Dhvani])
AM_INIT_AUTOMAKE
AC_PROG_CC
PKG_CHECK_MODULES(ALSA, [alsa >= 1.0.13],  ,
    [AC_MSG_ERROR([You must have the alsa dev libraries to build dhvani.]) ] 
)
CFLAGS="-g -O"
AC_PATH_PROGS(BASH, bash)
AC_CONFIG_FILES([
Makefile
man/Makefile
])
AC_PROG_INSTALL
AC_OUTPUT
