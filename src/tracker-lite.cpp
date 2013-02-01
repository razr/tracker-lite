#include <iostream>
#include <boost/bind.hpp>
#include "file-metadata-extractor.h"
#include "taglib-metadata-extractor.h"
#include "filesystem-scanner.h"
#include "database.h"
#include "thread-pool.h"
#include "file-database-persistor.h"


#define MAX_FILES_IN_TRANSACTION 360
#define MAX_ATTRIBUTES 12
#define PRAGMA_SYNC false




#ifdef WITH_STATISTICS
static int foundFiles = 0;
static int processedFiles = 0;
#endif


#ifdef WITH_MD5_IDS
std::string md5( const std::string inputStr )
{
	MD5_CTX ctx;
	unsigned char md[MD5_DIGEST_LENGTH ];
	MD5_Init(&ctx);
	int remaining = inputStr.length();
	while( remaining )
	{
		int toUpdate;
		if( remaining <= 512 )
		{
			toUpdate = remaining;
		}
		else
		{
			toUpdate = 512;
		}
		MD5_Update(&ctx, inputStr.c_str(), toUpdate );
		remaining -= toUpdate;
	}
	MD5_Final(md, &ctx);

	std::ostringstream outstream;
	outstream.setf( ios::hex, ios::basefield );
	for( int i = 0; i < MD5_DIGEST_LENGTH ; ++i )
	{
		outstream.fill('0');
		outstream.width(2);
		outstream  << (unsigned)md[i];
	}
	//cout << outstream.str().length();
	assert( outstream.str().length() == 2* MD5_DIGEST_LENGTH );
	return outstream.str();
}
#endif

#if 0
void cleanDataBase()
{

	execStatement("CREATE TABLE IF NOT EXISTS, files ( id INTEGER PRIMARY KEY AUTOINCREMENT, path VARCHAR(4096), type VARCHAR(10) ); " , true);
	execStatement("CREATE TABLE IF NOT EXISTS  attributes ( id VARCHAR(20), file_id VARCHAR(20), attribute_name VARCHAR(50), attribute_value VARCHAR(255) , FOREIGN KEY(file_id) REFERENCES filesystem(id) );", true);
	execStatement("CREATE INDEX fk_file_id ON attributes (file_id ASC);",  true);
	execStatement("CREATE INDEX fk_attribute_name ON attributes (attribute_name ASC);", true);
	execStatement("CREATE INDEX fk_attribute_id ON attributes (attribute_value ASC);", true);
	execStatement("DELETE FROM filesystem;");
	execStatement("DELETE FROM attributes;");

#ifdef PRAGMA_SYNC
	if( PRAGMA_SYNC )
		execStatement("PRAGMA synchronous = 1;", true);
	else
		execStatement("PRAGMA synchronous = 0;", true);
#else
	execStatement("PRAGMA synchronous = 1;", true);
#endif

	//	execStatement("PRAGMA temp_store = 2;");

}
#endif


using namespace std;



int main()
{

#ifdef __arm__
	std::string dbFile = "/aufs/opt/media/bin/trackertest";
#else
	std::string dbFile = "trackertest";
#endif


	cout << "MAX_FILES_IN_TRANSACTION = " << MAX_FILES_IN_TRANSACTION << endl;
	cout << "MAX_ATTRIBUTES = " 		  << MAX_ATTRIBUTES << endl;
	cout << "PRAGMA_SYNC = " 			  << PRAGMA_SYNC << endl;


	TagLibMetadataExtractor taglibExtractor;
	MetadataExtractManager::getInstance().registerExtractor( "MP3",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "WAV",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AAC",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "M4A",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "MP4",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "FLAC", &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "WMA",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AMR",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AWB",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "OGG",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "OGA",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "MP2",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "M4B",  &taglibExtractor);

	Database database( dbFile );

	try
	{
		database.checkAndCreateTables();
	}
	catch( const Database::Error & error )
	{
		cerr << "Fatal Error : " << error.getMessage() << endl;
		return -1;
	}

#ifdef WITH_STATISTICS
	time_t start_time, curr_time;
	time( &start_time );
#endif

	ThreadPool threadPool;
	FileDatabasePersistor  persistor( database );
	FileSystemScanner      fsScanner( boost::bind(&ThreadPool::pushFile, &threadPool, _1) );

	try
	{
		threadPool.start ( boost::bind( &FileDatabasePersistor::saveFile, &persistor, _1));
	}
	catch( ThreadPool::Error& error )
	{
		cerr << "Fatal Error : " << error.getMessage() << endl;
		return -2;
	}





	try
	{
		fsScanner.startExctractFolderRecursively("/media/BDB4-A56F");
	}
	//catch( const FileSystemScanner::Error& error )
	catch(...)
	{
		//cerr << "FatalError : " << error.getMessage() << endl;
		cerr << "FatalError " << endl;
		return -3;
	}

#ifdef WITH_STATISTICS
	cout << "Finished filesystem scan files in " << ( int ) ( time( &curr_time ) - start_time ) << " s.  found: " << foundFiles << endl;
#endif

	threadPool.terminate(ThreadPool::WAIT_ALL);

#ifdef WITH_STATISTICS
	cout << "Metatata extracted in " << ( int ) ( time( & curr_time ) - start_time ) << " s. " << endl;
	cout << "Found : " << allDataBaseEntries.size() << " entries " << endl;
	if( allDataBaseEntries.size() )
		cout << "Speed: " << allDataBaseEntries.size() / ( int ) ( time(&curr_time) - start_time ) << " files/s" << endl ;
#endif
	return 0;
}

