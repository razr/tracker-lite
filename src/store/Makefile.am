## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-store.la
libtlite_store_la_CFLAGS = $(GIO_CFLAGS) $(SQLITE3_CFLAGS) \
					 -I$(top_srcdir)/src/ce-devices \
					 -I$(top_srcdir)/src/miners/lib \
					 -I$(top_srcdir)/src/crawlers/lib \
					 -I$(top_srcdir)/src/extractors
libtlite_store_la_LIBADD = $(GIO_LIBS) $(SQLITE3_LIBS) \
					$(top_srcdir)/src/ce-devices/libtlite-ce-device.la \
					$(top_srcdir)/src/miners/lib/libtlite-miner.la
libtlite_store_la_SOURCES = tlite-store-object.c
