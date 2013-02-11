ACLOCAL_AMFLAGS = -I build-aux


EXTRA_DIST=AUTHORS NEWS INSTALL README COPYING ChangeLog


bin_PROGRAMS = trackerlite
trackerlite_CPPFLAGS = -Wall $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GLIB_THREADS_CFLAGS) $(TAGLIB_CFLAGS) $(SQLITE_CFLAGS) $(ICU_CFLAGS)
trackerlite_CPPFLAGS += -I src/common -I src/database -I src/devicemanager -I src/filesystem -I src/metadata -I src/metadata/extractors -I src/
trackerlite_SOURCES = src/tracker-lite.cpp \
					  src/database/database.cpp \
					  src/database/file-database-persistor.cpp \
					  src/devicemanager/device-manager.cpp \
				      src/metadata/file-metadata-extractor.cpp \
				      src/metadata/metadata-extract-pool.cpp \
				      src/metadata/extractors/taglib-metadata-extractor.cpp \
				      src/filesystem/filesystem-scanner.cpp \
				      src/common/string-encoding.cpp \
				      src/common/statistics.cpp \
				      src/common/logging.cpp \
				      src/common/console-logging.cpp
				      
trackerlite_LDADD = $(GLIB_LIBS) $(GIO_LIBS) $(GLIB_THREADS_LIBS) $(TAGLIB_LIBS) $(SQLITE_LIBS) $(ICU_LIBS)
