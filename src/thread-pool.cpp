/*
 * thread-pool.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */


#include "thread-pool.h"
#include "file-metadata.h"
#include "file-metadata-extractor.h"

#include "config.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <algorithm>

#define WITH_LOGGING

#ifdef WITH_LOGGING
#include <iostream>
#endif

void * ThreadPool::threadFunction( void * arg )
{
	std::pair<ThreadPool*, int> * argPair = static_cast< std::pair<ThreadPool*, int> * > ( arg );
	ThreadPool *instance = argPair->first;
	int        threadId  = argPair->second;
	delete argPair;
#ifdef WITH_LOGGING
	std::cout << "starting thread # " << threadId << std::endl;
#endif
	instance->runThread( threadId );
	return NULL;
}


void ThreadPool::runThread( int threadId )
{

	bool threadRunning = true;
	while( threadRunning )
	{
		pthread_mutex_lock( & m_queueMutexes[ threadId ]);
		if( m_queues[threadId].empty())
		{
			// when b_runThread is set to false
			// outside the thread, first wait queues empty
			if( ! b_runThread[threadId] )
				threadRunning = false;
			pthread_mutex_unlock( & m_queueMutexes[ threadId ]);
			/* wait for file crawler*/
			usleep(300); /*TODO - wait for semaphore ? */
		}
		else
		{
			std::string fileName =  m_queues[ threadId ].front();
			m_queues[ threadId ].pop();
#ifdef WITH_STATISTICS
			++ popped[threadId];
#endif
			pthread_mutex_unlock( & m_queueMutexes[ threadId ]);

			// extract metadata then insert in database
			processFile(fileName, threadId);

#ifdef WITH_STATISTICS
			processedFiles++;
#endif
			sleep(0);
		}
	}
}

void ThreadPool::start(FileMetadataExtractedFunctionType onFileMetadataExtractedFunc)
{
	m_onFileMetadataFunction = onFileMetadataExtractedFunc;
	for( int i = 0; i < THREAD_POOL_SIZE ; ++ i )
	{
		pthread_mutex_init( & m_queueMutexes[i], NULL );
		b_runThread[i] = true;
#ifdef WITH_STATISTICS
		pushed[i] = 0;
		poped[i] = 0;
#endif
		std::pair<ThreadPool*, int> *pair = new std::pair<ThreadPool*, int>(this, i);
		pthread_create( & m_threads[i], NULL, threadFunction, pair);
	}
}

void ThreadPool::terminate( const ThreadPool::TerminateMode mode)
{
	for( int i = 0; i < THREAD_POOL_SIZE ; ++ i )
	{
		b_runThread[i] = false;
	}

	if( mode == WAIT_ALL )
	{
		for( int i = 0; i < THREAD_POOL_SIZE ; ++ i )
		{
			pthread_join( m_threads[i], NULL );
		}
	}
}


void ThreadPool::pushFile( const std::string& file )
{
	static int threadID = 0;
	pthread_mutex_lock( & m_queueMutexes[ threadID ]);
	m_queues[ threadID ].push( file );
#ifdef WITH_STATISTICS
	++ pushed[threadID];
#endif
	pthread_mutex_unlock( & m_queueMutexes[ threadID ]);
	threadID = ( threadID + 1 ) % THREAD_POOL_SIZE;
}


void ThreadPool::processFile( const std::string& filePath, int threadId )
{
	std::cout << "thread #" << threadId << ": processFile " << filePath;
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
#ifdef WITH_LOGGING
    	std::cout << " Artist [" << f->m_metadata.m_artist << "]";
    	std::cout << " Album [" << f->m_metadata.m_album << "]";
    	std::cout << " Title [" << f->m_metadata.m_title << "]";
    	std::cout << " Composer [" << f->m_metadata.m_composer << "]";
    	std::cout << " Genre [" << f->m_metadata.m_genre << "]" << std::endl;
#endif
    	if( ! m_onFileMetadataFunction.empty())
    		m_onFileMetadataFunction( f );
    }
    catch( MetadataExtractManager::NoExtractorFoundError &error )
    {
#ifdef WITH_LOGGING
    	std::cout << " error :" << error.getMessage() << std::endl;
#endif
    }
    catch( AbstractFileMetadataExtractor::ExtractMetadataError &error )
    {
#ifdef WITH_LOGGING
    	std::cout << " metadata failed: " << error.getMessage() << std::endl;
#endif
    }
}
