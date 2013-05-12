#ifndef __TLITE_METADATA_INFO_H__
#define __TLITE_METADATA_INFO_H__

#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _TLiteMetadataInfo TLiteMetadataInfo;

GType                 tlite_metadata_info_get_type               (void) G_GNUC_CONST;

TLiteMetadataInfo *  tlite_metadata_info_new                    (GFile              *file);
TLiteMetadataInfo *  tlite_metadata_info_ref                    (TLiteMetadataInfo *info);
void                 tlite_metadata_info_unref                  (TLiteMetadataInfo *info);
GFile *              tlite_metadata_info_get_file               (TLiteMetadataInfo *info);

G_END_DECLS

#endif /* __TLITE_METADATA_INFO_H__ */
