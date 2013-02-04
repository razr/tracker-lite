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


DeviceManager::DeviceManager()
{
	m_deviceId = "";
	m_devicePath = "";
	m_fileSystemScanner.setOnFileFoundHandler(boost::bind(&ThreadPool::pushFile, &m_metadataExtractingPool, _1) );
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
		m_metadataExtractingPool.terminate( ThreadPool::FORCE_TERMINATE);
		m_database.close();
		m_database.open( "tracker_db_" + deviceId);
		m_metadataExtractingPool.start( boost::bind( &FileDatabasePersistor::saveFile, m_filePersistor, _1) );
		m_fileSystemScanner.startExctractFolderRecursively( devicePath );
	}
	catch( ThreadPool::Error& error )
	{
		std::cerr << "Fatal Error : " << error.getMessage() << std::endl;
	}
}

