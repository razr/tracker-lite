bin_PROGRAMS = trackerlite
trackerlite_CPPFLAGS = -Wall $(TAGLIB_CFLAGS) $(SQLITE_CFLAGS) $(ICU_I18N_CFLAGS) $(OPEN_SSL_CFLAGS)
trackerlite_SOURCES = tracker-lite.cpp \
				      database.cpp \
				      device-manager.cpp \
				      file-database-persistor.cpp \				      
				      file-metadata.cpp \
				      file-metadata-extractor.cpp \
				      filesystem-scanner.cpp \
				      string-encoding.cpp \
				      taglib-metadata-extractor.cpp \
				      thread-pool.cpp
				      
trackerlite_LDADD = $(TAGLIB_LIBS) $(SQLITE_LIBS) $(ICU_I18N_LIBS) $(OPEN_SSL_LIBS) -lpthread


