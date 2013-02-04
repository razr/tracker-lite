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
	FileFunctionType m_onFileFound;
	TerminateFunctionType m_onScanTerminated;
	void scanFolder( const std::string& folderPath);
	static void *scanThread( void* );
	bool m_scanRunning;
	pthread_t m_thread;
	bool      b_runThread;
	std::string m_folderToScan;
public:
	FileSystemScanner();
	void setOnFileFoundHandler(FileFunctionType onFileFound);
	void setOnFolderScanTerminated( TerminateFunctionType onScanTerminated );
	void  startScanFolderRecursively( const std::string& folderName );
	void cancelScan();
};



#endif /* FILESYSTEM_SCANNER_H_ */