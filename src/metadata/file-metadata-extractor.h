/*
 * file-metadata-extractor.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut.neicu@windriver.com
 */

#ifndef FILE_METADATA_EXTRACTOR_H_
#define FILE_METADATA_EXTRACTOR_H_

#include "file-metadata.h"
#include <map>

/**
 * @class AbstractFileMetadataExtractor
 * @brief abstract interface for metadata extractors
 * interface to be implemented by all metadata extractors
 * usually there are different extractors that will handle one or
 * more filetypes
 */
class AbstractFileMetadataExtractor
{
public:
	/**
	 * @class
	 * exception thrown by metadata extractors
	 */
	class ExtractMetadataError
	{
		File		m_file;
		std::string m_reason;
	public:
		ExtractMetadataError( const File& file, const std::string& reason );
		virtual std::string getMessage();
	};
	/**
	 * @brief extract metadata and fill the argument structure
	 * @param file that contains path on disk
	 * the resulted metadata will be stored in argument's m_metadata field
	 */
	virtual void extractMetadata( File& file )  const throw( ExtractMetadataError ) = 0;
};


/**
 * @class MetadataExtractManager
 * @brief singleton that registers extractors by file extension and decide which extractor to be used for a praticular file
 */
class MetadataExtractManager
{
	std::map<std::string, const AbstractFileMetadataExtractor*> m_registeredExtractors; //!< registered extractors by file extension
	static MetadataExtractManager m_instance; //!< singleton's instance
public:
	/**
	 * @class NoExtractorFoundError
	 * @brief exception thrown when no extractor found for a particular file
	 */
	class NoExtractorFoundError
	{
		std::string m_fileExtension;
	public:
		NoExtractorFoundError( const std::string& fileExtension );
		std::string getMessage();
	};
	/**
	 * @brief singleton's getInstance()
	 * @return always same instance
	 */
	static MetadataExtractManager& getInstance();
	/**
	 * @brief register extractor for specific file extension
	 * @param fileExtension - uppercase file extension ( eg. MP3 )
	 * @param extractor - extractor to be used for specified extractor
	 */
	void registerExtractor( const std::string& fileExtension, const AbstractFileMetadataExtractor* extractor );
	/**
	 * @brief looks for an extractor for specified file extension
	 * @param fileExtension - file extension to look up, uppercase
	 * @return extractor if found, or throws exception
	 */
	const AbstractFileMetadataExtractor* getMetadataExtractorForFile( const std::string& fileExtension ) const throw (NoExtractorFoundError);
};

#endif /* CONTENT_EXTRACTOR_H_ */
