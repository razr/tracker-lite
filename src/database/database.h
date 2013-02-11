/*
 * database.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <glib.h>
#include <sqlite3.h>
#include <string>

class Database;


class DatabaseStatement
{
	friend class Database;
private:
	sqlite3_stmt *m_stmt;
	sqlite3		 *m_dbConn;
	int 		  m_state;
	DatabaseStatement(sqlite3* dbConn, sqlite3_stmt *stmt); // only database can create statements;
public:

	virtual void bindInt   ( unsigned int paramNumber, int value );
	virtual void bindString( unsigned int paramNumber, const std::string& value );
	void exec();
	bool isDone();
	void nextRow();
	std::string				 getFieldAsString( const std::string& fieldName );
	gint64					 getFieldAsInteger( const std::string& fieldName );
	void release();
};

/**
 * @class Database
 * @brief Database interface for SQLite3 database used to store metadata
 */
class Database
{
	std::string m_fileName; //!< database file name
	sqlite3 *m_dbConn; //!< database connection obiect
	GMutex   m_mutextWriteLock; //!< write mutex ( database connection should not be used from different thread simultaneously )
	bool m_inTransaction; //!< database is in transaction
public:
	/**
	 * @brief Database exception
	 */
	class Error
	{
		std::string m_message; //!< error message
	public:
		/**
		 * @brief constructor
		 * @param message error message
		 */
		Error( const std::string& message )
		{
			m_message = message;
		}

		/**
		 * @brief error message
		 * @return the error message
		 */
		virtual std::string getMessage() const
		{
			return "Database error : " + m_message;
		}
	};
protected:
	/**
	* @brief create 'Folders' table if not exists, at startup
	*/
	void checkAndcreateFoldersTable() throw( Error );
	/**
	* @brief create 'Files' table if not exists, at startup
	*/
	void checkAndcreateFilesTable() throw( Error );
	/**
	* @brief create 'Titles' table if not exists, at startup
	*/
	void checkAndcreateTitlesTable() throw( Error );
	/**
	* @brief create 'Artists' table if not exists at startup
	*/
	void checkAndcreateArtistsTable() throw( Error );
	/**
	* @brief create 'Albums' table if not exists at startup
	*/
	void checkAndcreateAlbumsTable() throw( Error );
	/**
	* @brief create 'Composers' table if not exists at startup
	*/
	void checkAndcreateComposersTable() throw( Error );
	/**
	* @brief create 'Genres' table if not exists at startup
	*/
	void checkAndcreateGernesTable() throw( Error );
	/**
	* @brief creates all tables, indexes and all required Database objects if they don't exists
	*/
	void checkAndCreateTables() throw( Error );
public:
	/**
	 * @brief locks the database handle for write so concurrent statements are serialized
	 */
	void writeLock();
	/**
	* @brief unlocks the database handle after write
	*/
	void writeUnlock();

	/**
	 * @brief begins transaction
	 * if transaction already started, does nothing
	 */
	void beginTransaction() throw( Error );
	/**
	 * @brief commits active transaction, if any
	 * if no active transaction, does nothing
	 */
	void commitTransaction() throw( Error );
	/**
	 * @brief roolback active transaction, if any
	 * if no active transaction, does nothing
	 */
	void rollbackTransaction() throw( Error );
	/**
    * @brief constructor
	*/
	Database();
	/**
    * @brief constructor
	*/
	~Database();
	/**
	 * @brief open database
	 * @param fileName the file name to be used
	 */
	void open( const std::string& fileName ) throw ( Error );
	/**
	 * @brief executes INSERT or UPDATE
	 * @param sql to be executed
	 * @return the 'last insert id' if there is one
	 */
	gint64 executeInsertOrUpdate( const std::string& sql ) throw( Error );

	/**
	 * prepare select statement
	 * @param sql
	 * @return
	 */
	DatabaseStatement* prepareStatement( const std::string& sql ) throw( Error );
	/**
	 * @brief closes the database
	 */
	void close();

};


#endif /* DATABASE_H_ */
