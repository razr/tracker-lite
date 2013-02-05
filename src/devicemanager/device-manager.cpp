/*
 * device-manager.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */


#include <sstream>
#include <boost/bind.hpp>

#include "device-manager.h"
#include "database.h"

DeviceManager DeviceManager::m_instance;

#ifndef MAX_DEVICES_TO_KEEP_IN_HISTORY
#define MAX_DEVICES_TO_KEEP_IN_HISTORY 4
#endif

#define WITH_LOGGING
#ifdef WITH_LOGGING
#include <iostream>
#endif
#define WITH_STATISTICS
#ifdef WITH_STATISTICS
#include "statistics.h"
#endif


DeviceManager::DeviceManager()
{
	m_deviceId = "";
	m_devicePath = "";
	m_fileSystemScanner.setOnFileFoundHandler(boost::bind(&ThreadPool::pushFile, &m_metadataExtractingPool, _1) );
	m_fileSystemScanner.setOnFolderScanTerminated( boost::bind(&DeviceManager::fileSystemScanTerminated, this) );
	m_filePersistor = new FileDatabasePersistor(  m_database );
}


DeviceManager& DeviceManager::getInstance()
{
	return m_instance;
}

void DeviceManager::handleDeviceInserted(const std::string& deviceId, const std::string& devicePath)
{
	m_deviceId   = deviceId;
	m_devicePath = devicePath;
	//TODO: add new device in database, remove older ones if there are more than MAX_DEVICES_TO_KEEP_IN_HISTORY
	try
	{
		m_fileSystemScanner.cancelScan();
		m_metadataExtractingPool.terminate( ThreadPool::WAIT_ALL);
		m_filePersistor->flush();
		m_database.close();
#ifdef __arm__
		m_database.open( "/var/tracker_db_" + deviceId);
#else
		m_database.open( "tracker_db_" + deviceId);
#endif
		m_metadataExtractingPool.start( boost::bind( &FileDatabasePersistor::saveFile, m_filePersistor, _1) );
		m_fileSystemScanner.startScanFolderRecursively( devicePath );
	}
	catch( ThreadPool::Error& error )
	{
		std::cerr << "Fatal Error : " << error.getMessage() << std::endl;
	}
	catch( Database::Error& error )
	{
		std::cerr << "Fatal Error : " << error.getMessage() << std::endl;
	}
}

void DeviceManager::fileSystemScanTerminated(void)
{
	std::cout << "FS terminated" << std::endl;
#ifdef WITH_STATISTICS
	Statistics::getInstance().fsScanFinished();
#endif
	m_metadataExtractingPool.terminate( ThreadPool::WAIT_ALL);
	m_filePersistor->flush();
#ifdef WITH_STATISTICS
	Statistics::getInstance().metadataExtractFinished();
	Statistics::getInstance().print();
#endif

}
