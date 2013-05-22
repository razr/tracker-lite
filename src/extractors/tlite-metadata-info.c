#include <glib.h>
#include <gio/gio.h>

#include "taglib-metadata-extractor.h"
#include "tlite-metadata-info.h"

enum {
	TITLE,
	ALBUM,
	ARTIST,
	GENRE,
	COMPOSER,
	LAST_METADATA_FIELD
};

struct _TLiteMetadataInfo
{
	char *path;
	gchar *metadata[LAST_METADATA_FIELD];
	gint ref_count;
};

G_DEFINE_BOXED_TYPE (TLiteMetadataInfo, tlite_metadata_info,
                     tlite_metadata_info_ref, tlite_metadata_info_unref)

TLiteMetadataInfo *
tlite_metadata_info_new (GFile       *file)
{
	TLiteMetadataInfo *info;

	g_return_val_if_fail (G_IS_FILE (file), NULL);

	info = g_slice_new0 (TLiteMetadataInfo);

	extractor_get_metadata (file, info->metadata);
	info->path = g_file_get_path (file);

	info->ref_count = 1;

	return info;
}

TLiteMetadataInfo *
tlite_metadata_info_ref (TLiteMetadataInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	g_atomic_int_inc (&info->ref_count);

	return info;
}

void
tlite_metadata_info_unref (TLiteMetadataInfo *info)
{
	int i;

	g_return_if_fail (info != NULL);

	if (g_atomic_int_dec_and_test (&info->ref_count)) {
		for (i = 0; i < LAST_METADATA_FIELD; i++)
			g_free (info->metadata[i]);

		g_slice_free (TLiteMetadataInfo, info);
	}
}


char *
tlite_metadata_info_get_path (TLiteMetadataInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->path;
}

gchar **
tlite_metadata_info_get_metadata (TLiteMetadataInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->metadata;
}
