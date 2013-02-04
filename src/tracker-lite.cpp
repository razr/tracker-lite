#include <glib.h>
#include <iostream>
#include <boost/bind.hpp>
#include "file-metadata-extractor.h"
#include "taglib-metadata-extractor.h"
#include "device-manager.h"

#define MAX_FILES_IN_TRANSACTION 360
#define MAX_ATTRIBUTES 12
#define PRAGMA_SYNC false






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


// emulate new device
gboolean newDeviceAdded( gpointer ptr)
{
	DeviceManager::getInstance().handleDeviceInserted("BDB4-A56F_2", "/media/BDB4-A56F");
	return FALSE;
}

int main()
{

#ifdef __arm__
	std::string dbFile = "/aufs/opt/media/bin/trackertest";
#else
	std::string dbFile = "trackertest";
#endif

	GMainLoop *loop = g_main_loop_new ( NULL , FALSE );


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


	DeviceManager::getInstance().handleDeviceInserted("BDB4-A56F", "/media/BDB4-A56F");
	g_timeout_add_seconds( 10, newDeviceAdded, loop );
	g_main_loop_run (loop);
	g_main_loop_unref (loop);


#ifdef WITH_STATISTICS
	cout << "Finished filesystem scan files in " << ( int ) ( time( &curr_time ) - start_time ) << " s.  found: " << foundFiles << endl;
#endif



#ifdef WITH_STATISTICS
	cout << "Metatata extracted in " << ( int ) ( time( & curr_time ) - start_time ) << " s. " << endl;
	cout << "Found : " << allDataBaseEntries.size() << " entries " << endl;
	if( allDataBaseEntries.size() )
		cout << "Speed: " << allDataBaseEntries.size() / ( int ) ( time(&curr_time) - start_time ) << " files/s" << endl ;
#endif
	return 0;
}

