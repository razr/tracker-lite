## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-miner.la
libtlite_miner_la_CFLAGS = $(GLIB_CFLAGS) \
                     -I $(top_srcdir)/src/crawlers/lib \
                     -I $(top_srcdir)/src/ce-devices
libtlite_miner_la_LIBADD = $(GLIB_LIBS) \
                     $(top_srcdir)/src/crawlers/lib/libtlite-crawler.la
libtlite_miner_la_SOURCES = tlite-miner-object.c
