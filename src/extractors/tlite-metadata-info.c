#include <glib.h>

#include "taglib-metadata-extractor.h"
#include "tlite-metadata-info.h"

struct _TLiteMetadataInfo
{
	GFile *file;

	gchar *title;
	gchar *album;
	gchar *artist;
	gchar *genre;
	gchar *composer;

	gint ref_count;
};

G_DEFINE_BOXED_TYPE (TLiteMetadataInfo, tlite_metadata_info,
                     tlite_metadata_info_ref, tlite_metadata_info_unref)

TLiteMetadataInfo *
tlite_metadata_info_new (GFile       *file)
{
	TLiteMetadataInfo *info;
	GError *error = NULL;

	g_return_val_if_fail (G_IS_FILE (file), NULL);

	info = g_slice_new0 (TLiteMetadataInfo);
	info->file = g_object_ref (file);

	get_metadata (info->file, &info->artist);

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
	g_return_if_fail (info != NULL);

	if (g_atomic_int_dec_and_test (&info->ref_count)) {
		g_object_unref (info->file);

		g_free (info->title);
		g_free (info->album);
		g_free (info->artist);
		g_free (info->genre);
		g_free (info->composer);

		g_slice_free (TLiteMetadataInfo, info);
	}
}


GFile *
tlite_metadata_info_get_file (TLiteMetadataInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->file;
}
