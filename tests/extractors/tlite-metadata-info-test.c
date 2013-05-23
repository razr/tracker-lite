#include <glib.h>

#include <tlite-metadata-info.h>

static void
test_extract_info_setters (void)
{
        TLiteMetadataInfo *info;
        GFile *file;
		gchar **metadata;

        file = g_file_new_for_path ("./dummy.mp3");

        info = tlite_metadata_info_new (file);
        metadata = tlite_metadata_info_get_metadata (info);

 //       g_assert_cmpstr ("./dummy.mp3", ==, tlite_metadata_info_get_path (info));

        g_assert_cmpstr (metadata[0], ==, "Dummy Title");
        g_assert_cmpstr (metadata[1], ==, "Dummy Album");
        g_assert_cmpstr (metadata[2], ==, "Dummy Artist");
        g_assert_cmpstr (metadata[3], ==, "Dummy Genre");
//        g_assert_cmpstr (metadata[4], ==, "Dummy Composer");

        tlite_metadata_info_unref (info);

        g_object_unref (file);
}

int
main (int argc, char **argv)
{
        g_test_init (&argc, &argv, NULL);

        g_test_add_func ("/extractors/extract-info/setters",
                         test_extract_info_setters);

        return g_test_run ();
}
