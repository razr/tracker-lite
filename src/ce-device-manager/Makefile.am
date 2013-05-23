include $(top_srcdir)/Makefile.decl

noinst_LTLIBRARIES = libtlite-ce-device-manager.la
libtlite_ce_device_manager_la_CFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) \
					-I$(top_srcdir)/src/extractors \
					-I$(top_srcdir)/src/ce-devices \
					-I$(top_srcdir)/src/subsystems/lib \
					-I$(top_srcdir)/src/crawlers/lib \
					-I$(top_srcdir)/src/miners/lib \
					-I$(top_srcdir)/src/store
libtlite_ce_device_manager_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS) \
                     $(top_srcdir)/src/ce-devices/libtlite-ce-device.la \
                     $(top_srcdir)/src/subsystems/lib/libtlite-subsystem.la \
                     $(top_srcdir)/src/crawlers/lib/libtlite-crawler.la \
					 $(top_srcdir)/src/miners/lib/libtlite-miner.la \
					 $(top_srcdir)/src/store/libtlite-store.la
libtlite_ce_device_manager_la_SOURCES = ce-device-manager.c
