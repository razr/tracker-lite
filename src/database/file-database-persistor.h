/*
 * file-database-persistor.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef FILE_DATABASE_PERSISTOR_H_
#define FILE_DATABASE_PERSISTOR_H_

#include "database.h"
#include "file-metadata.h"

#include <list>
#include <pthread.h>

/**
 * files to be saved in one transaction
 */
#define FILES_PER_TRANSACTION 5
/**
 * @class FileDatabasePersistor
 * @brief persistor for file metadata
 */
class FileDatabasePersistor
{
	Database& m_database; 			//!< database used to save metadata
	std::list<File*> m_queuedFiles; //!< queued files to be saved in one transaction, @see FILES_PER_TRANSACTION
	pthread_mutex_t m_queue_mutex;
public:
	/**
	 * @brief persistence error
	 */
	class FilePersistenceError
	{
		std::string m_message; //!<< error message

	public:
		/**
		 * @brief constructor
		 * @param file file where error occurred
		 * @param reason error reason
		 */
		FilePersistenceError( const File& file, const std::string& reason )
		{
			m_message = "could not save file : " + file.m_stat.m_path + "reason : " + reason;
		}
		/**
		 * @brief constructor
		 * @param message the error message
		 */
		FilePersistenceError( const std::string& message )
		{
			m_message = message;
		}
		/**
		 * @brief returns the error message
		 * @return
		 */
		std::string getMessage()
		{
			return m_message;
		}
	};
	/**
	 * @brief generic save metadata for tables with same structure: title, artist, album, genre, composer
	 * @param metadataValue string value to be saved
	 * @param fieldName which table field to be used in DB
	 * @param tableName which table to be used in db
	 * @param f file object from ID is used to create one to one relation with file table
	 */
	virtual void genericSaveMetadata(const std::string& metadataValue,
								     const std::string& fieldName,
								     const std::string& tableName,
								     const File& f ) throw ( FilePersistenceError );
protected:
	/**
	 *@brief starts one or multiple files save in database
	 *locks the database for write
	 *starts DB transaction
	 */
	virtual void beginSave() throw ( FilePersistenceError );
	/**
	 * @brief saves main data in 'files' table - one row and generates id for File struct
	 * @param f file to be saved
	 */
	virtual void saveFileMainDataAndGetDBId(  File &f ) throw ( FilePersistenceError );
	/**
	 * @brief saves title detail in separate table
	 * saves title detail in separate table after main data saved in 'files' an id was generated
	 * @param f file to be saved
	 */
	virtual void saveTitle( const File &f ) throw ( FilePersistenceError );
	/**
	* @brief saves artist detail in separate table
	* saves artist detail in separate table after main data saved in 'files' an id was generated
	* @param f file to be saved
	*/
	virtual void saveArtist( const File &f ) throw ( FilePersistenceError );
	/**
	* @brief saves album detail in separate table
	* saves album detail in separate table after main data saved in 'files' an id was generated
	* @param f file to be saved
	*/
	virtual void saveAlbum( const File &f ) throw ( FilePersistenceError );
	/**
	* @brief saves genre detail in separate table
	* saves genre detail in separate table after main data saved in 'files' an id was generated
	* @param f file to be saved
	*/
	virtual void saveGenre( const File &f ) throw ( FilePersistenceError );
	/**
	* @brief saves genre detail in separate table
	* saves genre detail in separate table after main data saved in 'files' an id was generated
	* @param f file to be saved
	*/
	virtual void saveComposer( const File &f ) throw ( FilePersistenceError );
	/**
	* @brief saves composer detail in separate table
	* saves composer detail in separate table after main data saved in 'files' an id was generated
	* @param f file to be saved
	*/
	virtual void commitSave() throw ( FilePersistenceError );

	void saveCachedFiles(const std::list<File*>& m_filesToSave);
public:
	/**
	 * @brief constructor
	 * @param database
	 */
	FileDatabasePersistor( Database& database );
	/**
	 * @brief saves one file in database
	 * @param f file to be saved
	 */
	void saveFile( File* f ) throw ( FilePersistenceError );
	// loadFileByPathAndTimeStamp( File& f) throw ( FilePersistenceError );

	/**
	 * @brief saves all cached files
	 *
	 */
	void flush();
};



#endif /* FILE_DATABASE_PERSISTOR_H_ */
