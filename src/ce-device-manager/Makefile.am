## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-ce-device-manager.la
libtlite_ce_device_manager_la_CFLAGS = $(GUDEV_CFLAGS) $(GIO_CFLAGS) $(SQLITE3_CFLAGS) \
					-I$(top_srcdir)/src/crawlers/lib
libtlite_ce_device_manager_la_LIBADD = $(GUDEV_LIBS) $(GIO_LIBS) $(SQLITE3_LIBS) \
                     $(top_srcdir)/src/crawlers/lib/libtlite-crawler.la
libtlite_ce_device_manager_la_SOURCES = ce-device.c ce-device-manager.c
