include $(top_srcdir)/Makefile.decl

bin_PROGRAMS = tlite
tlite_SOURCES = tlite.c
tlite_CFLAGS = $(GIO_CFLAGS) $(GUDEV_CFLAGS) \
                        -I$(top_srcdir)/src/ce-device-manager
tlite_LDADD = $(GIO_LIBS) $(GUDEV_LIBS) \
                     $(top_srcdir)/src/ce-device-manager/libtlite-ce-device-manager.la
