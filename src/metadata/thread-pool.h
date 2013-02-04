/*
 * thread-pool.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <pthread.h>
#include <string>
#include <queue>
#include <boost/function.hpp>

#define THREAD_POOL_SIZE 8

class File;

typedef boost::function<void(File*)> FileMetadataExtractedFunctionType;

class ThreadPool
{
	std::queue<std::string> m_queues[THREAD_POOL_SIZE];
	pthread_mutex_t         m_queueMutexes[ THREAD_POOL_SIZE];
	bool				    b_runThread[ THREAD_POOL_SIZE ];
	pthread_t 			    m_threads[ THREAD_POOL_SIZE ];
	/*stats*/
#ifdef WITH_STATISTICS
	int pushed[THREAD_POOL_SIZE];
	int poped[THREAD_POOL_SIZE];
#endif
	FileMetadataExtractedFunctionType m_onFileMetadataFunction;
	static void* threadFunction( void * arg );
	void runThread(int threadId);
	void processFile( const std::string& filePath, int threadId );
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
				return "ThreadPool error : " + m_message;
			}
		};

	enum TerminateMode{ WAIT_ALL, FORCE_TERMINATE };
	void start( FileMetadataExtractedFunctionType onFileMetadataExtractedFunc);
	void terminate( const TerminateMode mode);
	void pushFile( const std::string& fileName );
};


#endif /* THREAD_POOL_H_ */
