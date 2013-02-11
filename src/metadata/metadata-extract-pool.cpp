/*
 * thread-pool.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut.neicu@windriver.com
 */


#include "metadata-extract-pool.h"
#include "file-metadata.h"
#include "file-metadata-extractor.h"

#include "config.h"
#include "logging.h"

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <algorithm>

#define WITH_STATISTICS
#ifdef  WITH_STATISTICS
#include "statistics.h"
#endif


MetadataExtractPool::MetadataExtractPool()
{
	m_threadPool = NULL;
}

void MetadataExtractPool::threadFunction( gpointer arg, gpointer userData )
{
	MetadataExtractPool *instance = static_cast<MetadataExtractPool*>( userData );
	char*       filePath = ( char *) arg;
	GThread 	*thread =  g_thread_self();
	LOG( LOG_VERBOSE, "starting extracting metadata for file %s on thread %x", filePath, (int)thread );
#ifdef WITH_STATISTICS
	Statistics::getInstance().newFileProcessedByThread( (int)thread  );
#endif
	instance->processFile( filePath, (int)thread );
	free( filePath );
#ifdef WITH_STATISTICS
	Statistics::getInstance().newFileProcessed();
#endif
}



void MetadataExtractPool::start() throw( MetadataExtractPool::Error)
{
	LOG(LOG_VERBOSE, "%s", __FUNCTION__);
	GError *glibError = NULL;
	m_threadPool = g_thread_pool_new( &MetadataExtractPool::threadFunction, this, THREAD_POOL_SIZE, TRUE, &glibError );
	if( glibError != NULL)
	{
		Error error( glibError->message );
		g_error_free( glibError );
	}
}

void MetadataExtractPool::terminate( const MetadataExtractPool::TerminateMode mode)
{
	LOG(LOG_VERBOSE, "%s mode = %d", __FUNCTION__ , mode );
	if( m_threadPool != NULL )
	{
		gboolean  wait = ( mode == MetadataExtractPool::WAIT_ALL );
		gboolean  terminateImmediately = FALSE;
		g_thread_pool_free( m_threadPool, terminateImmediately, wait );
	}
	m_threadPool = NULL;
}


void MetadataExtractPool::pushFile( const std::string& file )
{
	LOG(LOG_VERBOSE, "%s file = %s", __FUNCTION__ , file.c_str() );
	GError *error = NULL;
	g_thread_pool_push(m_threadPool, strdup( file.c_str() ), &error );
	if( error != NULL )
	{
		LOG( LOG_ERROR, "cannot push file in pool : %s", error->message );
	}
}


void MetadataExtractPool::processFile( const std::string& filePath, int threadId )
{
	LOG(LOG_VERBOSE, "%s filePath = %s threadId = %d", __FUNCTION__ , filePath.c_str() , threadId );
	std::string extension;
	size_t last_dot_pos = filePath.find_last_of(".");
	if (last_dot_pos != std::string::npos)
	{
		extension = filePath.substr(++last_dot_pos);
		std::transform( extension.begin(), extension.end(), extension.begin(), ::toupper );
	}
    try
    {
    	const AbstractFileMetadataExtractor* metadataExtractor = MetadataExtractManager::getInstance().getMetadataExtractorForFile( extension );

    	// if there is extractor, first stat file and fill up stat structure
    	File *f = new File;
    	f->m_stat.m_path = filePath;
    	struct stat fileStat;
    	if(  stat( filePath.c_str(), &fileStat ) == 0 )
    	{
    		f->m_stat.m_created   = fileStat.st_ctim.tv_sec;
    		f->m_stat.m_modified  = fileStat.st_mtim.tv_sec;
    		f->m_stat.m_size      = fileStat.st_size;
    	}

    	metadataExtractor->extractMetadata(*f);

    	LOG(LOG_DEBUG, "extracted artist  [%s] for file [%s] in thread [%x]", f->m_metadata.m_artist.c_str(), f->m_stat.m_path.c_str(), (int) g_thread_self() );
    	LOG(LOG_DEBUG, "extracted album   [%s] for file [%s] in thread [%x]", f->m_metadata.m_album.c_str(), f->m_stat.m_path.c_str(), (int) g_thread_self()  );
    	LOG(LOG_DEBUG, "extracted title   [%s]  for file [%s] in thread [%x]", f->m_metadata.m_title.c_str() , f->m_stat.m_path.c_str(), (int) g_thread_self() );
    	LOG(LOG_DEBUG, "extracted composer[%s] for file [%s] in thread [%x]", f->m_metadata.m_composer.c_str(), f->m_stat.m_path.c_str(), (int) g_thread_self()  );
    	LOG(LOG_DEBUG, "extracted genre   [%s] for file [%s] in thread [%x]", f->m_metadata.m_genre.c_str(), f->m_stat.m_path.c_str(), (int) g_thread_self()  );

    	for( std::list<MetadataExtractPoolObserver *>::iterator iter = m_observers.begin(); iter != m_observers.end(); ++ iter )
    	{
    			(*iter)->onFileMetadataExtracted(f);
    	}
    }
    catch( MetadataExtractManager::NoExtractorFoundError &error )
    {
    	LOG( LOG_WARN, "cannot extract metadata %s",  error.getMessage().c_str());
    }
    catch( AbstractFileMetadataExtractor::ExtractMetadataError &error )
    {
    	LOG( LOG_ERROR, "cannot extract metadata %s",  error.getMessage().c_str());
    }
}

void MetadataExtractPool::addObserver( MetadataExtractPoolObserver *observer )
{
	m_observers.push_back(observer);

}

void MetadataExtractPool::removeObserver( MetadataExtractPoolObserver *observer )
{
	m_observers.remove(observer);
}
