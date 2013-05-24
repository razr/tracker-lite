#ifndef __TAGLIB_METADATA_EXTRACTOR_H__
#define __TAGLIB_METADATA_EXTRACTOR_H__

#include <gio/gio.h>

#ifdef __cplusplus 
extern "C" { 
#endif

void extractor_get_metadata (GFile *file, gchar *metadata[]);

#ifdef __cplusplus 
}
#endif


#endif /* __TAGLIB_METADATA_EXTRACTOR_H__ */