/*
 * statistics.h
 *
 *  Created on: Feb 4, 2013
 *      Author: ionut
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <time.h>
#include <map>

#define WITH_STATISTICS
#ifdef WITH_STATISTICS


/**
 * @class Statistics
 * @brief optional singleton class that store statistics about number of file
 * 	      processes and mean execution time
 */
class Statistics
{
	static Statistics m_instance; //!<singleton instance
	Statistics(); //!< private constructor
	std::map<int, int> m_processedByThread; //! number of files de-queued and processed ( popped ) by each thread  map key = threadID, map value = number of files
	int m_allFilesScanned; //!< all files found in filesystem
	int m_allFilesProcessed; //!< all files processed ( metadata extracted if possible )
	time_t m_startTime; //!< indexing start time
	time_t m_fsScanEndTime; //!< indexing end time
	time_t m_metadataExtractEndTime; //!< metadata extraction end time, for all files and threads

public:
	/**
	 * @brief retunrs alwys single instance
	 * @return singletons's instance
	 */
	static Statistics& getInstance();
	/**
	 * @brief reset statistics and start new one
	 * 		  time measurements will start here also
	 */
	void reset();
	/**
	 * @brief called when new file metadata is being processed
	 */
	void newFileProcessed();

	/**
	 * @brief called when new file is found on filesystem, metadata not yet extracted
	*/
	void newFileScanned();

	/**
	* @brief called when filesystem scan finished and all files queued for metadata extraction
	*/
	void fsScanFinished();
	/**
	* @brief called all metadata extracted and all metadata thread queues empty
	*/
	void metadataExtractFinished();

	/**
	* @brief new file popped from thread's queue for processing
	* @param threadID the id of the thread that is being processing file
	*/
	void newFileProcessedByThread( int threadID );

	/**
	 * @brief print the statistics
	 */
	void print();

};
#endif

#endif /* STATISTICS_H_ */
