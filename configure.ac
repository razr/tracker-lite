#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tracker-lite], [0.1], [razr@github.com])
AM_INIT_AUTOMAKE([foreign])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([src/server/tlite.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES([GLIB], [glib-2.0])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])

PKG_CHECK_MODULES([GTHREAD], [gthread-2.0])
AC_SUBST([GTHREAD_CFLAGS])
AC_SUBST([GTHREAD_LIBS])

PKG_CHECK_MODULES([GMODULE], [gmodule-2.0])
AC_SUBST([GMODULE_CFLAGS])
AC_SUBST([GMODULE_LIBS])

PKG_CHECK_MODULES([GIO], [gio-2.0] [gio-unix-2.0])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES([GUDEV], [gudev-1.0])
AC_SUBST(GUDEV_CFLAGS)
AC_SUBST(GUDEV_LIBS)

PKG_CHECK_MODULES([TAGLIB_C], [taglib_c])
AC_SUBST(TAGLIB_C_CFLAGS)
AC_SUBST(TAGLIB_C_LIBS)

PKG_CHECK_MODULES([SQLITE3], [sqlite3])
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(SQLITE3_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/ce-devices/Makefile
                 src/ce-device-manager/Makefile
                 src/crawlers/Makefile
                 src/crawlers/lib/Makefile
                 src/extractors/Makefile
                 src/miners/Makefile
                 src/miners/lib/Makefile
                 src/subsystems/Makefile
                 src/subsystems/lib/Makefile
                 src/server/Makefile])
AC_OUTPUT
