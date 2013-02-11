/*
 * metadata-extract-pool.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut.neicu@windriver.com
 */

#ifndef METADATA_EXTRACT_POOL_H_
#define METADATA_EXTRACT_POOL_H_

#include <glib.h>
#include <string>
#include <list>


#define THREAD_POOL_SIZE 8

class File;


class MetadataExtractPoolObserver
{
public:
	virtual void onFileMetadataExtracted( File * f) = 0;
};


/**
 * @class ThreadPool
 * @brief ThreadPool used for metadata extraction
 * Metadata extraction is optionally parallelized using a number of threads
 * Each file is inserted in one's thread queue.
 * When one thread finishes metadata extraction will call (m_onFileMetadataFunction) callback
 * that will usually insert metadata in database
 */
class MetadataExtractPool
{

	GThreadPool *			m_threadPool;
	std::list<MetadataExtractPoolObserver *> m_observers;
	static void threadFunction( gpointer arg, gpointer userData ); //!< glib thread function
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
				return "MetadataExtractPool error : " + m_message;
			}
		};
	/**
	 * @brief constructor
	 */
	MetadataExtractPool();

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
	void start() throw( MetadataExtractPool::Error);
	/**
	 * add observer to thread pool
	 * @param observer
	 */
	void addObserver( MetadataExtractPoolObserver *observer );
	/**
	* remove observer to thread pool
	* @param observer
	*/
	void removeObserver( MetadataExtractPoolObserver *observer );
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
