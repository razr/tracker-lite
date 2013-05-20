#ifndef __TLITE_METADATA_INFO_H__
#define __TLITE_METADATA_INFO_H__

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _TLiteMetadataInfo TLiteMetadataInfo;

GType                 tlite_metadata_info_get_type               (void) G_GNUC_CONST;

TLiteMetadataInfo *  tlite_metadata_info_new                    (GFile       *file);
TLiteMetadataInfo *  tlite_metadata_info_ref                    (TLiteMetadataInfo *info);
void                 tlite_metadata_info_unref                  (TLiteMetadataInfo *info);
char *               tlite_metadata_info_get_path       		(TLiteMetadataInfo *info);
gchar **			 tlite_metadata_info_get_metadata			(TLiteMetadataInfo *info);

G_END_DECLS

#endif /* __TLITE_METADATA_INFO_H__ */
