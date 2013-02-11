/*
 * file-database-persistor.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef FILE_DATABASE_PERSISTOR_CPP_
#define FILE_DATABASE_PERSISTOR_CPP_

#include "file-database-persistor.h"
#include "logging.h"

#include <string>
#include <sstream>
#include <algorithm>


FileDatabasePersistor::FileDatabasePersistor( Database& database )
	: m_database(database)
{
	m_queue_mutex = g_mutex_new();
}

void FileDatabasePersistor::saveParentFolderOrGetId(File& f) throw( FileDatabasePersistor::FilePersistenceError)
{
	static const std::string selectStatement = "SELECT ID FROM folders WHERE folder_path = ?";
	static DatabaseStatement* statement = NULL;
	if( statement == NULL )
		statement = m_database.prepareStatement( selectStatement );
	try
	{
		statement->bindString(1, f.m_stat.m_folderPath);

		for( statement->exec(); ! statement->isDone(); statement->nextRow() )
		{
			f.m_db.m_folderId = statement->getFieldAsInteger("foldercount");
			break;
		}
		statement->release();
	}
	catch( Database::Error & error )
	{
		LOG(LOG_WARN, "database error %s", error.getMessage().c_str() );
	}

	// cannot find folder id in database, add new one:

	std::ostringstream sqlStream;
	sqlStream << "INSERT INTO folder( "
				<< "folder_path,"
				<< "folder_creation_time,"
				<< "file_modified_time,"
				<< "file_size"
				<< ") VALUES (" <<
					"'" << f.m_stat.m_path << "'" << "," <<
					f.m_stat.m_created  <<","<<
					f.m_stat.m_modified <<","<<
					f.m_stat.m_size
				<< ")";
	try
	{
		// execute update and set the file database unique ID
		f.m_db.m_folderId = m_database.executeInsertOrUpdate( sqlStream.str() );
	}
	catch( const Database::Error & error )
	{
		throw FilePersistenceError( f, "database error : " + error.getMessage() );
	}
}

void FileDatabasePersistor::saveFileMainDataAndGetDBId( File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	std::ostringstream sqlStream;
	sqlStream << "INSERT INTO files( "
				<< "file_path,"
				<< "file_creation_time,"
				<< "file_modified_time,"
				<< "file_size"
				<< ") VALUES (" <<
					"'" << f.m_stat.m_path << "'" << "," <<
					f.m_stat.m_created  <<","<<
					f.m_stat.m_modified <<","<<
					f.m_stat.m_size
				<< ")";
	try
	{
		// execute update and set the file database unique ID
		f.m_db.m_Id = m_database.executeInsertOrUpdate( sqlStream.str() );
	}
	catch( const Database::Error & error )
	{
		throw FilePersistenceError( f, "database error : " + error.getMessage() );
	}
}

void FileDatabasePersistor::genericSaveMetadata(const std::string& metadataValue,
												const  std::string& fieldName,
												const std::string & tableName,
												const File& f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	if( f.m_db.m_Id <= 0)
	{
		throw Database::Error("invalid file id");
	}

	if( metadataValue.empty() )
	{
		LOG(LOG_VERBOSE, "emtpy %s metadata for file %s", fieldName.c_str(), f.m_stat.m_path.c_str() );
		return;
	}

	try
	{
		std::ostringstream sqlStream;
		sqlStream << "INSERT INTO " << tableName <<" ( "
					<< "file_id,"
					<<  fieldName
					<< ") VALUES ( " 	<<
						f.m_db.m_Id 	<< ", " <<
						"'"<<metadataValue <<"'" <<
					" )";
		m_database.executeInsertOrUpdate( sqlStream.str() );
	}

	catch( const Database::Error & error )
	{
		LOG(LOG_ERROR, "error saving file %s : %s", f.m_stat.m_path.c_str(), error.getMessage().c_str()  );
		throw FilePersistenceError( f, "database error, " + error.getMessage() );
	}
}

void FileDatabasePersistor::saveTitle( const File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	genericSaveMetadata(f.m_metadata.m_title, "title_name", "titles", f);
}

void FileDatabasePersistor::saveArtist( const File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	genericSaveMetadata(f.m_metadata.m_artist, "artist_name", "artists", f);
}

void FileDatabasePersistor::saveAlbum( const File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	genericSaveMetadata(f.m_metadata.m_album, "album_name", "albums", f);
}
void FileDatabasePersistor::saveGenre( const File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	genericSaveMetadata(f.m_metadata.m_genre, "genre_name", "genres", f);
}
void FileDatabasePersistor::saveComposer( const File &f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	genericSaveMetadata(f.m_metadata.m_composer, "composer_name", "composers", f);
}

void FileDatabasePersistor::beginSave() throw ( FileDatabasePersistor::FilePersistenceError )
{

	m_database.writeLock();
	try
	{
		m_database.beginTransaction();
	}
	catch( const Database::Error & error )
	{
		throw FilePersistenceError( "database error : " + error.getMessage() );
	}
}

void FileDatabasePersistor::commitSave() throw ( FileDatabasePersistor::FilePersistenceError )
{
	try
	{
		m_database.commitTransaction();
		m_database.writeUnlock();
	}
	catch( const Database::Error & error )
	{
		m_database.writeUnlock();
		throw FilePersistenceError( "database error : " + error.getMessage() );
	}

}

void FileDatabasePersistor::saveFile( File* f ) throw( FileDatabasePersistor::FilePersistenceError)
{
	LOG(LOG_VERBOSE, "saving file : %s", f->m_stat.m_path.c_str() );
	std::list<File*> filesToSave;

	g_mutex_lock( m_queue_mutex );

	// just queue file :
	m_queuedFiles.push_back(f);
	// if number of queued files for save is big enough, save them
	if( m_queuedFiles.size() >= FILES_PER_TRANSACTION )
	{
		// push all data from m_queuedFiles to filesToSave and release m_queuedFiles for other trheads
		std::copy(m_queuedFiles.begin(), m_queuedFiles.end(), std::back_inserter(filesToSave) );
		m_queuedFiles.clear();
	}
	g_mutex_unlock( m_queue_mutex );

	if(filesToSave.size() > 0 )
		saveCachedFiles( filesToSave );
}


void FileDatabasePersistor::saveCachedFiles(const std::list<File*>& filesToSave)
{
	try
	{
			beginSave();
			for( std::list<File *>::const_iterator fiter = filesToSave.begin(); fiter != filesToSave.end(); ++fiter )
			{
				try
				{
					//saveParentFolderOrGetId(**fiter);
					saveFileMainDataAndGetDBId(**fiter);
					saveTitle(**fiter );
					saveAlbum(**fiter);
					saveArtist(**fiter);
					saveGenre(**fiter);
					saveComposer(**fiter);
					delete *fiter;
				}
				catch( FilePersistenceError& error)
				{
					LOG(LOG_ERROR, "error saving file in db %s", error.getMessage().c_str() );
				}
			}
			commitSave();
	}
	catch(FileDatabasePersistor::FilePersistenceError &error )
	{
			LOG(LOG_ERROR, "error saving file in db %s", error.getMessage().c_str() );
			m_database.rollbackTransaction();
			m_database.writeUnlock();
	}
}

void FileDatabasePersistor::flush()
{
	LOG(LOG_DEBUG, "flushing cached file for persistence found %d remaining files " , m_queuedFiles.size() );
	if( m_queuedFiles.size() > 0 )
		saveCachedFiles(m_queuedFiles);
}





#endif /* FILE_DATABASE_PERSISTOR_CPP_ */
