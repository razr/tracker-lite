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

Database::Database( const std::string& fileName )
{
	m_fileName = fileName;
	m_dbConn = NULL;
	pthread_mutex_init(& m_mutextWriteLock, NULL );
}

void Database::checkAndcreateDevicesTable() throw( Database::Error )
{

}

void Database::checkAndcreateFilesTable() throw( Database::Error )
{
	std::ostringstream sqlStream;
	sqlStream << "CREATE TABLE IF NOT EXISTS "
			  << "files ( "
			  << "file_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			  << "file_path VARCHAR(4096),"
			  << "file_device_id  	 INTEGER,"
			  << "file_creation_time INTEGER,"
			  << "file_modified_time INTEGER,"
			  << "file_size INTEGER"
			  <<")";
	executeInsertOrUpdate( sqlStream.str() );
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
}

void  Database::checkAndCreateTables() throw( Database::Error )
{

	int result = sqlite3_open( m_fileName.c_str(), &m_dbConn);
	if( result != SQLITE_OK )
	{
		std::string errMsg = std::string("can't open database: ") + sqlite3_errmsg(m_dbConn);
		sqlite3_close(m_dbConn);
		throw Error( errMsg );
	}

	try
	{
		writeLock();
		checkAndcreateFilesTable();
		checkAndcreateTitlesTable();
		checkAndcreateArtistsTable();
		checkAndcreateAlbumsTable();
		checkAndcreateComposersTable();
		checkAndcreateGernesTable();
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


