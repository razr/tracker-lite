include $(top_srcdir)/Makefile.decl

noinst_LTLIBRARIES = libtlite-crawler.la
libtlite_crawler_la_CFLAGS = $(GLIB_CFLAGS) \
					-I$(top_srcdir)/src/extractors \
					-I$(top_srcdir)/src/ce-devices
libtlite_crawler_la_LIBADD = $(GLIB_LIBS) \
                     $(top_srcdir)/src/ce-devices/libtlite-ce-device.la
libtlite_crawler_la_SOURCES = tlite-crawler-object.c
