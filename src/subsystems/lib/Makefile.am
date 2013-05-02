## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libtlite-subsystem.la
libtlite_subsystem_la_CFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GUDEV_CFLAGS) \
					-I$(top_srcdir)/src/ce-devices
libtlite_subsystem_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS) $(GUDEV_LIBS) \
                     $(top_srcdir)/src/ce-devices/libtlite-ce-device.la
libtlite_subsystem_la_SOURCES = tlite-subsystem-object.c
