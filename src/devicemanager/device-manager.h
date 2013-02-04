/*
 * device-manager.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_


#include "filesystem-scanner.h"
#include "database.h"
#include "thread-pool.h"
#include  "file-database-persistor.h"

// singleton
class DeviceManager
{
	static DeviceManager m_instance;
	DeviceManager();
	std::string m_deviceId;
	std::string m_devicePath;
	Database m_database;
	FileSystemScanner m_fileSystemScanner;
	ThreadPool		m_metadataExtractingPool;
	FileDatabasePersistor  *m_filePersistor;
	void fileSystemScanTerminated(void);
public:
	static DeviceManager& getInstance();
	void handleDeviceInserted(const std::string& deviceId, const std::string& devicePath);
	void onDeviceIndexingComplete();
};

#endif /* DEVICE_MANAGER_H_ */
