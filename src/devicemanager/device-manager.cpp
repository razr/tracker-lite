/*
 * device-manager.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#include <glib.h>
#include <sstream>
#include <boost/bind.hpp>

#include "device-manager.h"
#include "database.h"
#include "logging.h"

DeviceManager DeviceManager::m_instance;

#ifndef MAX_DEVICES_TO_KEEP_IN_HISTORY
#define MAX_DEVICES_TO_KEEP_IN_HISTORY 4
#endif

#define WITH_STATISTICS
#ifdef WITH_STATISTICS
#include "statistics.h"
#endif


DeviceManager::DeviceManager()
{
	m_deviceId = "";
	m_devicePath = "";
	m_fileSystemScanner.addObserver(this);
	m_metadataExtractingPool.addObserver(this);
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
		m_metadataExtractingPool.terminate( MetadataExtractPool::WAIT_ALL);
		m_filePersistor->flush();
		m_database.close();
#ifdef __arm__
		m_database.open( "/var/tracker_db_" + deviceId);
#else
		m_database.open( "tracker_db_" + deviceId);
#endif

		m_metadataExtractingPool.start();
		m_fileSystemScanner.startScanFolder( devicePath );
	}
	catch( MetadataExtractPool::Error& error )
	{
		LOG(LOG_ERROR, "metadata extract error %s", error.getMessage().c_str() );
	}
	catch( Database::Error& error )
	{
		LOG(LOG_ERROR, "database error %s", error.getMessage().c_str() );
	}
}

void DeviceManager::onFileFound( const std::string& fileName, bool isFolder, const std::string& parentFolder )
{
	LOG(LOG_VERBOSE, "file = %s", fileName.c_str() );
	if( parentFolder == m_devicePath )
	{
		if( ! isFolder )
			m_metadataExtractingPool.pushFile( parentFolder + "/" + fileName );
		else
			m_fileSystemScanner.startScanFolder( parentFolder + "/" + fileName );
	}


}

void DeviceManager::onFileMetadataExtracted( File * f)
{
	LOG(LOG_VERBOSE, "file = %s", f->m_stat.m_path.c_str() );
	m_filePersistor->saveFile(f);
}


void DeviceManager::onScanTerminated(void)
{
	LOG(LOG_DEBUG, "%s", "scan terminated");
#ifdef WITH_STATISTICS
	Statistics::getInstance().fsScanFinished();
#endif
	m_metadataExtractingPool.terminate( MetadataExtractPool::WAIT_ALL);
	m_filePersistor->flush();
#ifdef WITH_STATISTICS
	Statistics::getInstance().metadataExtractFinished();
	Statistics::getInstance().print();
#endif
	/* TODO remove */
	exit(0);

}
