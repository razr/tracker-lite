/*
 * device-manager.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

// singleton
class DeviceManager
{
	static DeviceManager m_instance;
	DeviceManager();
	std::string m_deviceId;
	std::string m_devicePath;
public:
	static DeviceManager& getInstance();
	void handleDeviceInserted(const std::string& deviceId, const std::string& devicePath);
};

#endif /* DEVICE_MANAGER_H_ */
