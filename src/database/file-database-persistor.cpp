/*
 * file-database-persistor.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef FILE_DATABASE_PERSISTOR_CPP_
#define FILE_DATABASE_PERSISTOR_CPP_

#include "file-database-persistor.h"

#include <string>
#include <sstream>
#include <algorithm>


#define WITH_LOGGING

#ifdef WITH_LOGGING
#include <iostream>
#endif

FileDatabasePersistor::FileDatabasePersistor( Database& database )
	: m_database(database)
{
	pthread_mutex_init( &m_queue_mutex, NULL );
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
#ifdef WITH_LOGGING
		std::cout << "emtpy [" << fieldName <<"] metadata for file :" << f.m_stat.m_path << std::endl;
#endif
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
		throw FilePersistenceError( f, "\n\ndatabase error : " + error.getMessage() );
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
		m_database.executeInsertOrUpdate( "BEGIN TRANSACTION;" );
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
		m_database.executeInsertOrUpdate( "COMMIT TRANSACTION;" );
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
	std::list<File*> filesToSave;

	pthread_mutex_lock(&m_queue_mutex);
	m_queuedFiles.push_back(f);
	if( m_queuedFiles.size() >= FILES_PER_TRANSACTION )
	{
		// push all data from m_queuedFiles to filesToSave and release m_queuedFiles for other trheads
		std::copy(m_queuedFiles.begin(), m_queuedFiles.end(), std::back_inserter(filesToSave) );
		m_queuedFiles.clear();
	}
	pthread_mutex_unlock(&m_queue_mutex);

	try
	{
			beginSave();
			for( std::list<File *>::iterator fiter = filesToSave.begin(); fiter != filesToSave.end(); ++fiter )
			{
				saveFileMainDataAndGetDBId(**fiter);
				saveTitle(**fiter );
				saveAlbum(**fiter);
				saveArtist(**fiter);
				saveGenre(**fiter);
				saveComposer(**fiter);
				delete *fiter;
			}
			commitSave();
	}
	catch(FileDatabasePersistor::FilePersistenceError &error )
	{
#ifdef WITH_LOGGING
			std::cout << "error saving file in db : " << error.getMessage() << std::endl;
#endif
			m_database.writeUnlock();
	}
}
#endif /* FILE_DATABASE_PERSISTOR_CPP_ */
