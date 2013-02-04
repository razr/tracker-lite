/*
 * file-metadata-extractor.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef FILE_METADATA_EXTRACTOR_H_
#define FILE_METADATA_EXTRACTOR_H_

#include "file-metadata.h"
#include <map>

class AbstractFileMetadataExtractor
{
public:
	class ExtractMetadataError
	{
		File		m_file;
		std::string m_reason;
	public:
		ExtractMetadataError( const File& file, const std::string& reason );
		virtual std::string getMessage();
	};
	virtual void extractMetadata( File& file )  const throw( ExtractMetadataError ) = 0;
};


// singleton
class MetadataExtractManager
{
	std::map<std::string, const AbstractFileMetadataExtractor*> m_registeredExtractors;
	static MetadataExtractManager m_instance;
public:
	class NoExtractorFoundError
	{
		std::string m_fileExtension;
	public:
		NoExtractorFoundError( const std::string& fileExtension );
		std::string getMessage();
	};

	static MetadataExtractManager& getInstance();
	void registerExtractor( const std::string& fileExtension, const AbstractFileMetadataExtractor* extractor );
	const AbstractFileMetadataExtractor* getMetadataExtractorForFile( const std::string& fileExtension ) const throw (NoExtractorFoundError);
};

#endif /* CONTENT_EXTRACTOR_H_ */
