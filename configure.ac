AC_INIT(trackerlite, 0.1, costel.grosu@windriver.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O

AC_CHECK_HEADERS([string.h]) 

### check for boost signals library
AX_BOOST_BASE([1.40])
AX_BOOST_SIGNALS

### check for taglib
PKG_CHECK_MODULES([TAGLIB], [taglib],[],[echo "taglib package required but not found"; exit 1])
AC_SUBST(TAGLIB_CFLAGS)
AC_SUBST(TAGLIB_LIBS)

### check for sqlite3
PKG_CHECK_MODULES([SQLITE], [sqlite3],[],[echo "sqlite package required but not found"; exit 1])
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)

### check for icu
PKG_CHECK_MODULES(ICU, [icu-i18n],[],[
	AX_CHECK_ICU([4.4.2],[], [echo "libicu package required but not found"; exit 1])
	])
AC_SUBST(ICU_CFLAGS)
AC_SUBST(ICU_LIBS)


AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug], [this option enable debug compile option]),
              [], [CFLAGS="-g -O0";CXXFLAGS="-g -O0"])

AC_CONFIG_FILES([
                Makefile
                src/Makefile
        ])

AC_OUTPUT


