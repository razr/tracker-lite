/*
 * statistics.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: ionut
 */

#include "statistics.h"
#include <iostream>

Statistics Statistics::m_instance;

Statistics::Statistics()
{
	reset();
}

Statistics& Statistics::getInstance()
{

	return m_instance;
}

void Statistics::reset()
{
	m_pushedByThread.clear();
	m_poppedByThread.clear();
	m_allFilesScanned = 0;
	m_allFilesProcessed = 0;
	m_startTime = time( NULL );
}

void Statistics::newFileProcessed()
{
	++m_allFilesProcessed;
}

void Statistics::newFileScanned()
{
	++m_allFilesScanned;
}



void Statistics::fsScanFinished()
{
	m_fsScanEndTime = time( NULL );
}

void Statistics::metadataExtractFinished()
{
	m_metadataExtractEndTime = time( NULL );
}

void Statistics::newFilePushed( int threadID )
{
	if( m_pushedByThread.find(threadID) == m_pushedByThread.end() )
		m_pushedByThread[threadID] = 1;
	else
		++m_pushedByThread[threadID];
}
void Statistics::newFilePopped( int threadID )
{
	if( m_poppedByThread.find(threadID) == m_poppedByThread.end() )
			m_poppedByThread[threadID] = 1;
	else
		++m_poppedByThread[threadID];
}

void Statistics::print()
{
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "Statistics " << std::endl;
	std::cout << "Files Scanned   : " << m_allFilesScanned << std::endl;
	std::cout << "Files Processed : " << m_allFilesProcessed << std::endl;
	std::cout << "Files Processed by Thread : "  << std::endl;
	for( std::map<int,int>::iterator iter = m_pushedByThread.begin(); iter != m_pushedByThread.end() ; ++ iter )
	{
		std::cout << "		Thread " << iter->first << " pushed : " << iter->second << std::endl;
	}
	std::cout << "Files Processed by Thread : "  << std::endl;
	for( std::map<int,int>::iterator iter = m_poppedByThread.begin(); iter != m_poppedByThread.end() ; ++ iter )
	{
			std::cout << "		Thread " << iter->first << " poped : " << iter->second << std::endl;
	}
	std::cout << "Duration : " << std::endl;
	std::cout << "     FileSystemScan 	  : " << m_fsScanEndTime - m_startTime << " seconds" << std::endl;
	std::cout << "     MetadataExtraction : " << m_metadataExtractEndTime - m_startTime << " seconds" << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "Averages : " << std::endl;
	if( m_fsScanEndTime - m_startTime )
		std::cout << "     FileSystemScan 	  : " << m_allFilesScanned / ( m_fsScanEndTime - m_startTime ) << " files/s" << std::endl;
	if( m_metadataExtractEndTime - m_startTime )
		std::cout << "     MetadataExtraction : " << m_allFilesProcessed / ( m_metadataExtractEndTime - m_startTime ) << " files/s" << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
}
