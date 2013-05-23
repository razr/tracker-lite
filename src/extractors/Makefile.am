include $(top_srcdir)/Makefile.decl

noinst_LTLIBRARIES = libtlite-extractor.la
libtlite_extractor_la_CFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) $(TAGLIB_C_CFLAGS) $(GCOV_CFLAGS)
libtlite_extractor_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS) $(TAGLIB_C_LIBS) $(GCOV_LIBS)
libtlite_extractor_la_SOURCES = taglib-metadata-extractor.c tlite-metadata-info.c
