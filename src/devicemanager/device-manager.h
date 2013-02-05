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
#include "file-database-persistor.h"

/**
 * @class DeviceManager
 * @brief handles device insertion
 * contains everything necessary to start scanning one device
 * usually, applications will call handleDeviceInserted() method and device extraction will start
 */
class DeviceManager
{
	static DeviceManager m_instance; //!< instance to be used
	std::string m_deviceId; //!< deviceId
	std::string m_devicePath; //!< device mounting root path
	Database m_database; //!< database to be used
	FileSystemScanner m_fileSystemScanner; //!< filesystem scanner
	ThreadPool		  m_metadataExtractingPool; //!< metadata extraction thread pool
	FileDatabasePersistor  *m_filePersistor; //!< file persistor
	/**
	 * @brief constructor
	 */
	DeviceManager();
	/**
	 * @brief filesystem scan terminate handler - used to generate DBUS messages and statistics
	 */
	void fileSystemScanTerminated(void);
	/**
	* @brief metadate extraction and database save terminate handler - used to generate DBUS messaages and statistics
	*/
	void onDeviceIndexingComplete();
public:
	/**
	 * @brief singleton's getInstance
	 * @return always the same instance
	 */
	static DeviceManager& getInstance();
	/**
	 * @brief starts device indexing
	 * @param deviceId will be used to generate database file
	 * @param devicePath device mounting root path
	 */
	void handleDeviceInserted(const std::string& deviceId, const std::string& devicePath);

};

#endif /* DEVICE_MANAGER_H_ */
