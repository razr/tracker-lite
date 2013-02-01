/*
 * content-extractor.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef CONTENT_EXTRACTOR_CPP_
#define CONTENT_EXTRACTOR_CPP_

#include "file-metadata-extractor.h"

AbstractFileMetadataExtractor::ExtractMetadataError::ExtractMetadataError( const File& file, const std::string& reason )
{
	m_file = file;
	m_reason = reason;
}

std::string AbstractFileMetadataExtractor::ExtractMetadataError::getMessage()
{
	return std::string("could not extract metadata for file :") + m_file.m_stat.m_path + std::string(", reason :") + m_reason;
}


MetadataExtractManager MetadataExtractManager::m_instance;

MetadataExtractManager::NoExtractorFoundError::NoExtractorFoundError( const std::string& fileExtension )
{
	m_fileExtension = fileExtension;
}

std::string MetadataExtractManager::NoExtractorFoundError::getMessage()
{
	return "no extractor found for file extension : " + m_fileExtension;
}

MetadataExtractManager& MetadataExtractManager::getInstance()
{
	return m_instance;
}


void MetadataExtractManager::registerExtractor( const std::string& fileExtension, const AbstractFileMetadataExtractor* extractor )
{
	// TODO thread synchronization
	m_registeredExtractors[fileExtension] = extractor;
}

const AbstractFileMetadataExtractor* MetadataExtractManager::getMetadataExtractorForFile( const std::string& fileExtension ) const throw (NoExtractorFoundError)
{
	// TODO thread synchronization
	std::map<std::string, const AbstractFileMetadataExtractor*>::const_iterator findIter =  m_registeredExtractors.find(fileExtension);
	if( findIter == m_registeredExtractors.end() )
		throw NoExtractorFoundError( fileExtension );
	return findIter->second;
}

#endif /* CONTENT_EXTRACTOR_CPP_ */
