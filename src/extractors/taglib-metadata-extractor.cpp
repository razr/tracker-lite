#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

#include <iostream>

#include <tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>

#include "taglib-metadata-extractor.h"

using namespace std;
using namespace TagLib::MPEG;
using namespace TagLib::ID3v2;

void
extractor_get_metadata (GFile *file,
                        gchar *metadata[])
{
	TagLib::MPEG::File f(g_file_get_path (file), false);
	TagLib::ID3v2::Tag *id3v2tag = f.ID3v2Tag();

	if (!id3v2tag)
		return;

	/* TODO: set own string handler setLatin1StringHandler for Asian */
	/* TODO: check whether it parses complete id3v2 tag? we need only 5 */
	/* TODO: why do we need g_strdup here? */

	metadata [0] = g_strdup (id3v2tag->title().toCString(true));
	metadata [1] = g_strdup (id3v2tag->album().toCString(true));
	metadata [2] = g_strdup (id3v2tag->artist().toCString(true));
	metadata [3] = g_strdup (id3v2tag->genre().toCString(true));

	TagLib::ID3v2::FrameList fl = id3v2tag->frameListMap()["TCOM"];
	if(!fl.isEmpty())
		metadata [4] = g_strdup (fl.front()->toString().toCString(true));
}
