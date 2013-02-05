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
/**
 * @class ThreadPool
 * @brief ThreadPool used for metadata extraction
 * Metadata extraction is optionally parallelized using a number of threads
 * Each file is inserted in one's thread queue.
 * When one thread finishes metadata extraction will call (m_onFileMetadataFunction) callback
 * that will usually insert metadata in database
 */
class ThreadPool
{
	std::queue<std::string> m_queues[THREAD_POOL_SIZE]; //!< thread queues - will receive file paths
	pthread_mutex_t         m_queueMutexes[ THREAD_POOL_SIZE]; //!< queue mutexes needed because queues are FIFOS across two threads
	bool				    b_runThread[ THREAD_POOL_SIZE ]; //!< pool threads running flag. set to false to terminate
	pthread_t 			    m_threads[ THREAD_POOL_SIZE ]; //!< threads object
	bool					b_threadsStarted[ THREAD_POOL_SIZE ];
	FileMetadataExtractedFunctionType m_onFileMetadataFunction; //!< callback to be called whrn metadata extracted
	static void* threadFunction( void * arg ); //!< posix thread function
	void runThread(int threadId); //!<< object's thread method
	void processFile( const std::string& filePath, int threadId ); //!< metadata extraction method
public:
	/**
	 * @class ThreadPool error
	 */
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
	/**
	 * @brief constructor
	 */
	ThreadPool();

	/**
	 * @enum TerminateMode
	 * Two ways to terminate thread pool
	 * 	WAIT_ALL will wait all thread queues to became empty
	 * 	FORCE_TERMINATE will terminate asap
	 */
	enum TerminateMode{ WAIT_ALL, FORCE_TERMINATE };
	/**
	 * start thread pool
	 * @param onFileMetadataExtractedFunc - callback to be called when file extracted
	 */
	void start( FileMetadataExtractedFunctionType onFileMetadataExtractedFunc);
	/**
	 * terminate thread pool
	 * @param mode - mode to terminate @see TerminateMode
	 */
	void terminate( const TerminateMode mode);
	/**
	 * push one file to one of the threads for metadata extraction
	 * @param fileName - complete path to file on filesystem
	 */
	void pushFile( const std::string& fileName );
};


#endif /* THREAD_POOL_H_ */
