include $(top_srcdir)/Makefile.decl

noinst_PROGRAMS = $(TEST_PROGS)

TEST_PROGS += tlite-metadata-info-test

tlite_metadata_info_test_SOURCES = tlite-metadata-info-test.c
tlite_metadata_info_test_CFLAGS = $(GLIB_CFLAGS) -I$(top_srcdir)/src/extractors
tlite_metadata_info_test_LDADD = $(GLIB_LIBS) $(top_builddir)/src/extractors/libtlite-extractor.la -lstdc++

EXTRA_DIST = dummy.mp3
