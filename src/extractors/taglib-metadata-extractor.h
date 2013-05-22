#ifndef __TAGLIB_METADATA_EXTRACTOR_H__
#define __TAGLIB_METADATA_EXTRACTOR_H__

#include <gio/gio.h>

G_BEGIN_DECLS

void extractor_get_metadata (GFile *file, gchar *metadata[]);

G_END_DECLS

#endif /* __TAGLIB_METADATA_EXTRACTOR_H__ */