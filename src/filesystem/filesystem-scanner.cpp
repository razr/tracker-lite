/*
 * filesystem-scanner.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */


#include "filesystem-scanner.h"
#include "file-metadata.h"
#include <string.h>
#include <gio/gio.h>




#include "logging.h"

#define WITH_STATISTICS
#ifdef  WITH_STATISTICS
#include "statistics.h"
#endif

FileSystemScanner::FileSystemScanner()
{
	b_runThread = false;
	m_foldersToScan = g_async_queue_new();
}


void FileSystemScanner::addObserver( FileSystemScannerObserver* observer )
{
	m_observers.push_back( observer );
}

gpointer FileSystemScanner::scanThread( gpointer userData )
{
	FileSystemScanner* pInstance = static_cast<FileSystemScanner *>( userData );

	while( pInstance->b_runThread )
	{
		gchar * folderToScan = (gchar *) g_async_queue_pop( pInstance->m_foldersToScan );
		if( folderToScan )
		{
			pInstance->scanFolder( folderToScan );
			g_free( folderToScan );
		}
		else
			usleep(100);
	}


	for( std::list<FileSystemScannerObserver*>::iterator iter = pInstance->m_observers.begin(); iter != pInstance->m_observers.end(); ++ iter )
	{
		(*iter)->onScanTerminated();
	}

	return NULL;
}

int64_t FileSystemScanner::getFileLastModified( const std::string& filePath )
{
  GFile *file;
  GFileInfo *info;
  GTimeVal time;
  int64_t result;

  file = g_file_new_for_path (filePath.c_str());

  info = g_file_query_info (file, "*",
                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, NULL);

  g_file_info_get_modification_time (info, &time);

  result = time.tv_sec;

  g_object_unref (file);
  g_object_unref (info);

  return result;
}

void FileSystemScanner::scanFolder( const std::string& folderName )
{
		LOG(LOG_DEBUG, "scanning folder  %s", folderName.c_str() );

		if( ! b_runThread )
			return;

		GError *error = NULL;
		GFile *file = g_file_new_for_path( folderName.c_str() );
		GFileInfo * fileInfo = NULL;

		if( ! file )
		{
			LOG(LOG_WARN,  "could not open file : %s", folderName.c_str() );
			return;
		}



		GFileEnumerator * enumerator =  g_file_enumerate_children( file,
														G_FILE_ATTRIBUTE_STANDARD_TYPE,
														G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		                                                NULL,
		                                                &error);
		if( error != NULL )
		{
			LOG(LOG_WARN, "could not enumerate folder content : %s", error->message );
			goto cleanup;
		}

		while( 1 )
		{
					fileInfo = g_file_enumerator_next_file(enumerator,
					                                         NULL,
					                                         &error );
					if( error != NULL )
					{
						LOG(LOG_WARN, "could not enumerate folder content %s", error->message );
						break;
					}
					if( fileInfo == NULL )
					{
						LOG(LOG_DEBUG, "terminate folder enumeration %", folderName.c_str() );
						break;
					}
					GTimeVal lastModified;
					GFileType   fileType = g_file_info_get_file_type( fileInfo);
					const char *fileName = g_file_info_get_name ( fileInfo );
					g_file_info_get_modification_time( fileInfo, &lastModified );
					int changed =0;
					//changed = getFileLastModified( std::string( folderName + "/" + fileName ).c_str() );
					//g_file_info_get_attribute_int64( fileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED );
					switch( fileType )
					{
						case G_FILE_TYPE_DIRECTORY:
							for( std::list<FileSystemScannerObserver*>::iterator iter = m_observers.begin(); iter != m_observers.end(); ++ iter )
							{
									(*iter)->onFileFound( fileName, true, folderName );
							}
						break;

						case G_FILE_TYPE_REGULAR:
							for( std::list<FileSystemScannerObserver*>::iterator iter = m_observers.begin(); iter != m_observers.end(); ++ iter )
							{
									(*iter)->onFileFound( fileName, false, folderName );
							}
						break;

						default:
							break;
					}
					if( fileInfo )
						g_object_unref( fileInfo );
					fileInfo = NULL;
		}
		g_file_enumerator_close(enumerator, NULL, &error);
		if( error != NULL )
		{
			LOG(LOG_WARN, "could not close enumerator %s", error->message );
		}
cleanup:
		if( file )
			g_object_unref( file );

		if( enumerator )
			g_object_unref( enumerator );

		if( fileInfo )
			g_object_unref( fileInfo );

}

void FileSystemScanner::startScanFolder( const std::string& folderName )
{
		LOG(LOG_VERBOSE, "folder name = %s", folderName.c_str() );
		if( ! b_runThread )
		{
			m_thread = g_thread_new(NULL, scanThread , this);
			if( m_thread != NULL )
			{
				b_runThread = false;
			}
			b_runThread = true;
		}

		g_async_queue_push( m_foldersToScan, g_strdup( folderName.c_str() ));


}

void FileSystemScanner::cancelScan()
{
	LOG(LOG_VERBOSE, " already running = %d", b_runThread );
	if( b_runThread )
	{
		b_runThread = false;
		g_thread_join( m_thread );
	}
}




