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
class Statistics
{
	static Statistics m_instance;
	Statistics();
	std::map<int,int> m_pushedByThread;
	std::map<int, int> m_poppedByThread;
	int m_allFilesScanned;
	int m_allFilesProcessed;
	time_t m_startTime;
	time_t m_fsScanEndTime;
	time_t m_metadataExtractEndTime;

public:
	static Statistics& getInstance();
	void reset();
	void newFileProcessed();
	void newFileScanned();

	void fsScanFinished();
	void metadataExtractFinished();

	void newFilePushed( int threadID );
	void newFilePopped( int threadID );

	void print();

};
#endif

#endif /* STATISTICS_H_ */
