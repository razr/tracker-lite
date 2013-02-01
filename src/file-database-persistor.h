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

class FileDatabasePersistor
{
	Database& m_database;

public:
	class FilePersistenceError
	{
		std::string m_reason;
		File m_file;
	public:
		FilePersistenceError( const File& file, const std::string& reason )
		{
			m_file = file;
			m_reason = reason;
		}
		std::string getMessage()
		{
			return "could not save file : " + m_file.m_stat.m_path + "reason : " + m_reason;
		}
	};
	virtual void genericSaveMetadata(const std::string& metadataValue,
								     const std::string& fieldName,
								     const std::string& tableName,
								     const File& f ) throw ( FilePersistenceError );
protected:
	virtual void beginSave() throw ( FilePersistenceError );
	virtual void saveFileMainDataAndGetDBId(  File &f ) throw ( FilePersistenceError );
	virtual void saveTitle( const File &f ) throw ( FilePersistenceError );
	virtual void saveArtist( const File &f ) throw ( FilePersistenceError );
	virtual void saveAlbum( const File &f ) throw ( FilePersistenceError );
	virtual void saveGenre( const File &f ) throw ( FilePersistenceError );
	virtual void saveComposer( const File &f ) throw ( FilePersistenceError );
	virtual void commitSave() throw ( FilePersistenceError );
public:
	FileDatabasePersistor( Database& database );
	void saveFile( File* f ) throw ( FilePersistenceError );
	// loadFileByPathAndTimeStamp( File& f) throw ( FilePersistenceError );
};



#endif /* FILE_DATABASE_PERSISTOR_H_ */
