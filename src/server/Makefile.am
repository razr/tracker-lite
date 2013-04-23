## Process this file with automake to produce Makefile.i

bin_PROGRAMS = tlite
tlite_SOURCES = tlite.c options.c options.h
tlite_CFLAGS = -Wall
tlite_CPPFLAGS = $(GIO_CFLAGS) $(GUDEV_CFLAGS) \
                        -I$(top_srcdir)/src/ce-device-manager
tlite_LDADD = $(GIO_LIBS) $(GUDEV_LIBS) \
                     $(top_srcdir)/src/ce-device-manager/libtlite-ce-device-manager.la
