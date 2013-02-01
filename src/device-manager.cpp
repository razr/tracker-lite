/*
 * device-manager.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */


#include <sstream>
#include "device-manager.h"
#include "database.h"

DeviceManager DeviceManager::m_instance;

#ifndef MAX_DEVICES_TO_KEEP_IN_HISTORY
#define MAX_DEVICES_TO_KEEP_IN_HISTORY 4
#endif

DeviceManager::DeviceManager()
{
	m_deviceId = "";
	m_devicePath = "";
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

}

