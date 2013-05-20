## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-ce-device.la
libtlite_ce_device_la_CFLAGS = $(GUDEV_CFLAGS) $(GIO_CFLAGS) $(SQLITE3_CFLAGS) \
					-I$(top_srcdir)/src/extractors
libtlite_ce_device_la_LIBADD = $(GUDEV_LIBS) $(GIO_LIBS) $(SQLITE3_LIBS) \
					$(top_srcdir)/src/extractors/libtlite-extractor.la
libtlite_ce_device_la_SOURCES = ce-device.c
