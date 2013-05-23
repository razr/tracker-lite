include $(top_srcdir)/Makefile.decl

SUBDIRS = src

if HAVE_UNIT_TESTS
SUBDIRS += tests
endif

EXTRA_DIST = ANNOUNCE BUGS FAQ README

MAINTAINERCLEANFILES=configure

if ENABLE_GCOV
.PHONY: coverage coverage-report
coverage:
	find $(top_srcdir) -name "*.gcda" | xargs rm -f
	$(MAKE) $(AM_MAKEFLAGS) test

coverage-report: coverage
	lcov --directory $(top_srcdir)/src --capture --output-file $(top_builddir)/lcov.info
	lcov --directory $(top_srcdir)/src --output-file $(top_builddir)/lcov.info --remove $(top_builddir)/lcov.info \
		extractors/tlite-metadata-info.c
	rm -rf $(top_builddir)/coverage
	$(mkdir_p) $(top_builddir)/coverage
	genhtml --title "@PACKAGE_STRING@" --output-directory $(top_builddir)/coverage $(top_builddir)/lcov.info
endif
