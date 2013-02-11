/*
 * file-metadata.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef FILE_METADATA_H_
#define FILE_METADATA_H_

#include <string>
#include <glib.h>

/**
 * @struct File
 * @brief  Value object used to store file information
 *		   It consists in 3 subgroups
 *		   		DB - contains the database storage information (id) after file is saved to
 *		   		FileSystem - information retrieved from filesystem ( with stat )
 *		   		Metadata   - media information extracted with format-specific methods
 *		   		note all this subgroups are filled in different moments by the application
 *		   		first FileSystem information is extracted, then Metadata and after file is
 *		   		saved in database, DB id is stored as well
 */
struct File
{
	/**
	 * @class DB
	 * @brief  database information
	 * @var DB::m_Id the id of file in database
	 *
	 */
	class DB
	{
	public:
		gint64 m_Id;
		gint64 m_folderId;
		/// constructor
		DB()
		{
			m_Id = 0; // not saved yet
			m_folderId = 0;
		}
	}m_db;

	/** @struct FileSystem
	 *  @brief  stores file system information for a file
	 */
	struct FileSystem
	{
		std::string m_path; //!< file path
		time_t		m_created; //!< creation time
		time_t		m_modified; //!< modified time
		gint64      m_size; //!< size of file, in bytes
		std::string m_folderPath; //!< parent folder path
		time_t		m_folderLastModified; //!< parent folder last modified
	} m_stat;

	/** @struct FileSystem
	 *  @brief  stores file system information for a file
	 */
	struct Metadata
	{
		std::string m_title; //!< title metadata
		std::string m_album; //!< album metadata
		std::string m_artist;//!< artist metadata
		std::string m_composer;//!< composer metadata
		std::string m_genre; //!< genre metadata
	} m_metadata;



};


#endif /* FILE_METADATA_H_ */
