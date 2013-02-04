/*
 * database.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <sqlite3.h>
#include <string>
#include <sys/types.h>
#include <pthread.h>

class Database
{
	std::string m_fileName;
	sqlite3 *m_dbConn;
	pthread_mutex_t   m_mutextWriteLock;
public:
	class Error
	{
		std::string m_message;
	public:
		Error( const std::string& message )
		{
			m_message = message;
		}

		virtual std::string getMessage() const
		{
			return "Database error : " + m_message;
		}
	};
protected:
	void checkAndcreateDevicesTable() throw( Error );
	void checkAndcreateFilesTable() throw( Error );
	void checkAndcreateTitlesTable() throw( Error );
	void checkAndcreateArtistsTable() throw( Error );
	void checkAndcreateAlbumsTable() throw( Error );
	void checkAndcreateComposersTable() throw( Error );
	void checkAndcreateGernesTable() throw( Error );
	void checkAndCreateTables() throw( Error );
public:
	void writeLock();
	void writeUnlock();
	Database();
	void open( const std::string& fileName ) throw ( Error );
	int64_t executeInsertOrUpdate( const std::string& sql ) throw( Error );
	void close();

};


#endif /* DATABASE_H_ */
