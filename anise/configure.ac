AC_PREREQ(2.53)

dnl Process this file with autoconf to produce a configure script.
#AC_INIT([Project ANISE], [beta8-20040216-linux], [tomyun@yonsei.ac.kr], anise)
#AC_CONFIG_SRCDIR(anise.h)
AC_INIT(anise.h)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialize for automake
#AM_INIT_AUTOMAKE(1.6)
AM_INIT_AUTOMAKE(anise, [beta8-20040216-linux])

dnl Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL

dnl Check sizes of integer types
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)

dnl Checks for libraries.
dnl Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION is not found]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Checks for library functions.

AC_OUTPUT(Makefile)
