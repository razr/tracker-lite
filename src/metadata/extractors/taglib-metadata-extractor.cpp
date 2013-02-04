/*
 * taglib-metadata-extractor.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#include "taglib-metadata-extractor.h"
#include "string-encoding.h"
#include "file-metadata.h"

#include <taglib/fileref.h>
#include <taglib/tag.h>

void TagLibMetadataExtractor::extractMetadata( File& file ) const throw( TagLibMetadataExtractor::ExtractMetadataError )
{
	TagLib::FileRef f( file.m_stat.m_path.c_str() );
	if( ! f.isNull() )
	{
		TagLib::String t_album = f.tag()->album();
		TagLib::String t_artist = f.tag()->artist();
		TagLib::String t_title = f.tag()->title();
		TagLib::String t_composer = f.tag()->artist();
		TagLib::String t_genre = f.tag()->genre();

		if( t_title.isLatin1() )
		{

			file.m_metadata.m_title    = convertStringToUTF8( t_title.toCString(false) );
		}
		else
		{
			// convert to UTF-8;
			file.m_metadata.m_title    = t_title.toCString(true);
		}

		if( t_artist.isLatin1() )
		{
			file.m_metadata.m_artist = convertStringToUTF8( t_artist.toCString(false) );
		}
		else
		{
			// convert to UTF-8;
			file.m_metadata.m_artist = t_artist.toCString(true);
		}

		if( t_album.isLatin1() )
		{
			file.m_metadata.m_album = convertStringToUTF8(t_album.toCString(false) );
		}
		else
		{
			// convert to UTF-8
			file.m_metadata.m_album = t_album.toCString(true);
		}


		if( t_composer.isLatin1() )
		{
			file.m_metadata.m_composer = convertStringToUTF8(t_composer.toCString(false) );
		}
		else
		{
			// convert to UTF-8
			file.m_metadata.m_composer = t_composer.toCString(true);
		}


		if( t_genre.isLatin1() )
		{
			file.m_metadata.m_genre = convertStringToUTF8(t_genre.toCString(false) );
		}
		else
		{
			// convert to UTF-8
			file.m_metadata.m_genre = t_genre.toCString(true);
		}

	}
	else
	{
		throw ExtractMetadataError( file, "not supported" );
	}
}



