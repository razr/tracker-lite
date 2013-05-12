## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-miner.la
libtlite_miner_la_CFLAGS = $(GIO_CFLAGS) \
					 -I $(top_srcdir)/src/crawlers/lib \
					 -I $(top_srcdir)/src/ce-devices \
					 -I $(top_srcdir)/src/extractors
libtlite_miner_la_LIBADD = $(GIO_LIBS) \
					$(top_srcdir)/src/crawlers/lib/libtlite-crawler.la \
					$(top_srcdir)/src/extractors/libtlite-extractor.la
libtlite_miner_la_SOURCES = tlite-miner-object.c
