## Process this file with automake to produce Makefile.i

bin_PROGRAMS = tlite
tlite_SOURCES = tlite.c
tlite_CFLAGS = $(GIO_CFLAGS) $(GUDEV_CFLAGS) $(GCOV_CFLAGS) \
                        -I$(top_srcdir)/src/ce-device-manager
tlite_LDADD = $(GIO_LIBS) $(GUDEV_LIBS) $(GCOV_LIBS) \
                     $(top_srcdir)/src/ce-device-manager/libtlite-ce-device-manager.la
