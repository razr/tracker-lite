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
AC_PROG_CXX
AC_PROG_INSTALL

AM_PROG_CC_C_O

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

PKG_CHECK_MODULES([TAGLIB], [taglib])
AC_SUBST(TAGLIB_CFLAGS)
AC_SUBST(TAGLIB_LIBS)

PKG_CHECK_MODULES([SQLITE3], [sqlite3])
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(SQLITE3_LIBS)

AC_ARG_ENABLE(unit_tests,
              AS_HELP_STRING([--enable-unit-tests],
                             [enable unit tests [[default=yes]]]), ,
              [enable_unit_tests=yes])

if test "x$enable_unit_tests" != "xno" ; then
	have_unit_tests=yes

	AC_DEFINE(HAVE_UNIT_TEST, 1, [Define if we have GLib unit test framework])

	AC_PATH_PROG(GTESTER,gtester,yes,no)
	if test $GTESTER == "no"; then
		AC_MSG_ERROR([Could not find 'gtester'])
	fi
else
   have_unit_tests=no
fi

AM_CONDITIONAL(HAVE_UNIT_TESTS, test "x$have_unit_tests" = "xyes")

AC_ARG_ENABLE(gcov,
              AS_HELP_STRING([--enable-gcov],
                             [enable coverage reporting support [[default=no]]]),,
              [enable_gcov=no])
AM_CONDITIONAL(ENABLE_GCOV, test "x$enable_gcov" != "xno")

if test "x$enable_gcov" != "xno" ; then
	AC_PATH_PROG(LCOV,lcov,yes,no)
	AC_PATH_PROG(GENHTML,genhtml,yes,no)

	if test "$LCOV" == "no" -o "$GENHTML" == "no"; then
		AC_MSG_ERROR(['lcov' and 'genhtml' are required for profiling.])
	fi

	GCOV_CFLAGS="-ftest-coverage -fprofile-arcs"
	GCOV_LIBS="-lgcov"
else
	GCOV_CFLAGS=
	GCOV_LIBS=
fi

AC_SUBST(GCOV_CFLAGS)
AC_SUBST(GCOV_LIBS)

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
                 src/store/Makefile
                 src/subsystems/Makefile
                 src/subsystems/lib/Makefile
                 src/server/Makefile
                 tests/Makefile
                 tests/extractors/Makefile])
AC_OUTPUT
