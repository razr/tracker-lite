## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-crawler.la
libtlite_crawler_la_CFLAGS = $(GLIB_CFLAGS) \
					-I$(top_srcdir)/src/ce-devices
libtlite_crawler_la_LIBADD = $(GLIB_LIBS) \
                     $(top_srcdir)/src/ce-devices/libtlite-ce-device.la
libtlite_crawler_la_SOURCES = tlite-crawler-object.c
