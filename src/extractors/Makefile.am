include $(top_srcdir)/Makefile.decl

noinst_LTLIBRARIES = libtlite-extractor.la
libtlite_extractor_la_CFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GCOV_CFLAGS)
libtlite_extractor_la_CXXFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) $(TAGLIB_CFLAGS) $(GCOV_CFLAGS)
libtlite_extractor_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS) $(TAGLIB_LIBS) $(GCOV_LIBS) -lstdc++
libtlite_extractor_la_SOURCES = taglib-metadata-extractor.cpp tlite-metadata-info.c
