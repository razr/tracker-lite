/*
 * file-metadata.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef FILE_METADATA_H_
#define FILE_METADATA_H_

#include <string>
#include <sys/types.h>

struct File
{
	struct DB
	{
		int64_t m_Id;
		DB()
		{
			m_Id = 0;
		}
	}m_db;

	struct FileSystem
	{
		std::string m_deviceId;
		std::string m_path;
		std::string m_name;
		std::string m_extension;
		time_t		m_created;
		time_t		m_modified;
		int64_t     m_size;
	} m_stat;

	struct Metadata
	{
		std::string m_title;
		std::string m_album;
		std::string m_artist;
		std::string m_composer;
		std::string m_genre;
	} m_metadata;



};


#endif /* FILE_METADATA_H_ */
