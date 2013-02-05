/*
 * filesystem-scanner.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef FILESYSTEM_SCANNER_H_
#define FILESYSTEM_SCANNER_H_

#include <string>
#include <boost/function.hpp>
#include <pthread.h>


typedef boost::function<void(const std::string&)> FileFunctionType;
typedef boost::function<void(void)> TerminateFunctionType;

/**
 * @class FileSystemScanner
 * @brief performs recursive scan for specified folder
 */
class FileSystemScanner
{
	FileFunctionType m_onFileFound; //!< on file found handler, called for each file found on filesystem
	TerminateFunctionType m_onScanTerminated; //!< callback to be called when folder recursive scan terminated
	/**
	 * @brief recursive scan one folder
	 * recursive scan one folder and for each file found, call external m_onFileFound callback
	 * this function will call itself for each subfolder
	 * @param folderPath path to scan
	 */
	void scanFolder( const std::string& folderPath);
	/**
	 * @brief pthread internal thread function
	 * @param userData user data to be passed to thread
	 */
	static void *scanThread( void* userData );

	pthread_t m_thread; //!< internat thread
	bool      b_runThread; //!< scan thread running, set to FALSE if scan need to be cancelled
	std::string m_folderToScan; //!< folder to be used as root for scanning
public:
	/**
	 * @brief constructor
	 */
	FileSystemScanner();
	/**
	 * @brief sets external "onFileFound" callback
	 * @param onFileFound - boost function type callback
	 */
	void setOnFileFoundHandler(FileFunctionType onFileFound);
	/**
	 * @brief sets extenral "onScanTerminated" callback
	 * @param onScanTerminated - boost function type callback
	 */
	void setOnFolderScanTerminated( TerminateFunctionType onScanTerminated );
	/**
	 * @brief starts scanning folder recursivelly
	 * this function will return immediately as all processing is made in separate thread
	 * @see setOnFileFoundHandler and setOnFolderScanTerminated that will set callbacks
	 * for file found and scan terminate respectively
	 * @param folderName - folder to be used as root for filesystem scan
	 */
	void  startScanFolderRecursively( const std::string& folderName );
	/**
	 * @brief cancel the filesystem scan
	 * cancel the background thread and waits for thread join
	 * so after cancelScan() retunrs, no callbacks will be called
	 */
	void cancelScan();
};



#endif /* FILESYSTEM_SCANNER_H_ */
