/*
 * database.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#include "database.h"
#include <sqlite3.h>
#include <sstream>

#define WITH_SQL_LOGGING


#ifdef WITH_SQL_LOGGING
#include <iostream>
#endif

Database::Database()
{
	m_dbConn = NULL;
	m_inTransaction = false;
	pthread_mutex_init(& m_mutextWriteLock, NULL );
}



void Database::checkAndcreateFilesTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
			  << "files ( "
			  << "file_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			  << "file_path VARCHAR(4096),"
			  << "file_creation_time INTEGER,"
			  << "file_modified_time INTEGER,"
			  << "file_size INTEGER"
			  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_files_id ON files (file_path, file_creation_time, file_modified_time ASC);" );
}
void Database::checkAndcreateTitlesTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
				  << "titles ( "
				  << "title_id INTEGER PRIMARY KEY AUTOINCREMENT,"
				  << "file_id   INTEGER,"
				  << "title_name    VARCHAR(256)"
				  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_titles_id ON titles (file_id ASC);" );
}
void Database::checkAndcreateArtistsTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
					  << "artists ( "
					  << "artist_id INTEGER PRIMARY KEY AUTOINCREMENT,"
					  << "file_id   INTEGER,"
					  << "artist_name    VARCHAR(256)"
					  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_artists_id ON artists (file_id  ASC);" );
}
void Database::checkAndcreateAlbumsTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
						  << "albums ( "
						  << "album_id INTEGER PRIMARY KEY AUTOINCREMENT,"
						  << "file_id  INTEGER,"
						  << "album_name    VARCHAR(256)"
						  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_albums_id ON albums (file_id ASC);" );
}
void Database::checkAndcreateComposersTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
						  << "composers ( "
						  << "composer_id INTEGER PRIMARY KEY AUTOINCREMENT,"
						  << "file_id   INTEGER,"
						  << "composer_name   VARCHAR(256)"
						  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_composers_id ON composers (file_id ASC);" );
}
void Database::checkAndcreateGernesTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
						  << "genres ( "
						  << "genre_id 		INTEGER PRIMARY KEY AUTOINCREMENT,"
						  << "file_id   	INTEGER,"
						  << "genre_name    VARCHAR(256)"
						  <<")";
	executeInsertOrUpdate( sqlStream.str() );
	executeInsertOrUpdate( "CREATE INDEX IF NOT EXISTS fk_genres_id ON genres (file_id ASC);" );
}

void  Database::checkAndCreateTables() throw( Database::Error )
{
	try
	{
		writeLock();
		checkAndcreateFilesTable();
		checkAndcreateTitlesTable();
		checkAndcreateArtistsTable();
		checkAndcreateAlbumsTable();
		checkAndcreateComposersTable();
		checkAndcreateGernesTable();
		executeInsertOrUpdate("PRAGMA synchronous = 0;");
		writeUnlock();
	}
	catch( Database::Error & error )
	{
		writeUnlock();
		throw;
	}
}

int64_t Database::executeInsertOrUpdate( const std::string& sql ) throw( Database::Error )
{
#ifdef WITH_SQL_LOGGING
		std::cout << " Execute INSERT/UPDATE SQL = " << sql << std::endl;
#endif
	char *errMsg = NULL;
	if( ! m_dbConn )
	{
		throw Database::Error("no database connection");
	}

	int rc = sqlite3_exec(m_dbConn, sql.c_str() , NULL, 0, &errMsg);
	if( rc != SQLITE_OK )
	{
		Database::Error error( errMsg );
		/* This will free zErrMsg if assigned */
		//if (errMsg)
		//   free(errMsg);
		throw error;
	}
	return sqlite3_last_insert_rowid(m_dbConn);
}

void Database::writeLock()
{
	pthread_mutex_lock(& m_mutextWriteLock );
}

void Database::writeUnlock()
{
	pthread_mutex_unlock(& m_mutextWriteLock );
}

void Database::open(const std::string& fileName ) throw (Database::Error)
{
		if( m_dbConn )
			close();
		m_fileName = fileName;
		int result = sqlite3_open( m_fileName.c_str(), &m_dbConn);
		if( result != SQLITE_OK )
		{
			std::string errMsg = std::string("can't open database: ") + sqlite3_errmsg(m_dbConn);
			sqlite3_close(m_dbConn);
			throw Error( errMsg );
		}
		checkAndCreateTables();
}

void Database::close()
{
	if( m_dbConn )
	{
		sqlite3_close( m_dbConn );
		m_dbConn = NULL;
		m_inTransaction = false;
	}
}

void Database::beginTransaction() throw( Error )
{
	if( ! m_inTransaction )
	{
		executeInsertOrUpdate("BEGIN TRANSACTION;");
		m_inTransaction = true;
	}
}

void Database::commitTransaction() throw( Error )
{
	if( m_inTransaction )
	{
		executeInsertOrUpdate("COMMIT TRANSACTION;");
		m_inTransaction = false;
	}
}

void Database::rollbackTransaction() throw( Error )
{
	if( m_inTransaction )
	{
		executeInsertOrUpdate("ROLLBACK TRANSACTION;");
		m_inTransaction = false;
	}
}
