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

### check for glib
PKG_CHECK_MODULES([GLIB], [glib-2.0],[],[echo "glib package required but not found"; exit 1])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

### check for icu
PKG_CHECK_MODULES(ICU, [icu-i18n],[],[
	AX_CHECK_ICU([4.4.2],[], [echo "libicu package required but not found"; exit 1])
	])
AC_SUBST(ICU_CFLAGS)
AC_SUBST(ICU_LIBS)



#PKG_CHECK_MODULES([GST_INTERFACES], [gstreamer-interfaces-0.10 >= 0.10])
#AC_SUBST(GST_INTERFACES_CFLAGS)
#AC_SUBST(GST_INTERFACES_LIBS)

#PKG_CHECK_MODULES([GST_APP], [gstreamer-app-0.10 >= 0.10])
#AC_SUBST(GST_APP_CFLAGS)
#AC_SUBST(GST_APP_LIBS)

#GST_REQUIRED=0.10.10
#PKG_CHECK_MODULES(GSTREAMER, [
#    gstreamer-0.10              >= $GST_REQUIRED
#    gstreamer-base-0.10         >= $GST_REQUIRED
#    gstreamer-interfaces-0.10   >= $GST_REQUIRED
#    gstreamer-pbutils-0.10      >= $GST_REQUIRED
#  ], [
#    AC_SUBST(GSTREAMER_CFLAGS)
#    AC_SUBST(GSTREAMER_LIBS)
#  ], [
#    AC_MSG_ERROR([
#    "Can't find the following GStreamer development packages:

#      gstreamer-0.10              >= $GST_REQUIRED
#      gstreamer-base-0.10         >= $GST_REQUIRED
#      gstreamer-interfaces-0.10   >= $GST_REQUIRED
#      gstreamer-pbutils-0.10      >= $GST_REQUIRED

#     Please make sure you have the necessary GStreamer-0.10
#     development headers installed."])
#])


### check for systemd watchdog support
#PKG_CHECK_MODULES(SYSTEMD, [ libsystemd-daemon >= 42 ], watchdog="yes", watchdog="no")
#if test "x$watchdog" = "xyes"; then
#    AC_DEFINE(BUILD_WITH_WATCHDOG, [], [Build watchdog support if systemd v42 or greater])
#    AC_SUBST(SYSTEMD_CFLAGS)
#    AC_SUBST(SYSTEMD_LIBS)
#fi

SUBDIRS = src
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug], [this option enable debug compile option]),
              [], [CFLAGS="-g -O0";CXXFLAGS="-g -O0"])

AC_CONFIG_FILES([
                Makefile   
                            
        ])
AC_OUTPUT


