#include <gio/gio.h>
#include <tag_c.h>

#include "taglib-metadata-extractor.h"

void
get_metadata (GFile *file,
              gchar *metadata[])
{
	TagLib_File *taglib_file;
	TagLib_Tag *taglib_tag;

	taglib_file = taglib_file_new_type (g_file_get_path (file), TagLib_File_MPEG);
	taglib_tag = taglib_file_tag (taglib_file);

	metadata [0] = taglib_tag_title (taglib_tag);
	metadata [1] = taglib_tag_album (taglib_tag);
	metadata [2] = taglib_tag_artist (taglib_tag);
	metadata [3] = taglib_tag_genre (taglib_tag);
	metadata [4] = taglib_tag_artist (taglib_tag);

//	taglib_tag_free_strings ();
//	taglib_file_free (taglib_file);
}
