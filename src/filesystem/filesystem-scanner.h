/*
 * filesystem-scanner.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef FILESYSTEM_SCANNER_H_
#define FILESYSTEM_SCANNER_H_

#include <string>
#include <list>
#include <glib.h>




class FileSystemScannerObserver
{
public:
	virtual void onFileFound( const std::string& fileName, bool isFolder, const std::string& parentFolder ) = 0;
	virtual void onScanTerminated() = 0;
};

/**
 * @class FileSystemScanner
 * @brief performs recursive scan for specified folder
 */
class FileSystemScanner
{
	std::list<FileSystemScannerObserver*> m_observers;
	/**
	 * @brief recursive scan one folder
	 * recursive scan one folder and for each file found, call external m_onFileFound callback
	 * this function will call itself for each subfolder
	 * @param folderPath path to scan
	 */
	void scanFolder( const std::string& folderPath);
	/**
	 * @brief internal thread function
	 * @param userData user data to be passed to thread
	 */
	static gpointer scanThread( gpointer userData );

	GThread*  m_thread; //!< internat thread
	bool      b_runThread; //!< scan thread running, set to FALSE if scan need to be cancelled
	GAsyncQueue *m_foldersToScan; // !< queue for folders to scan
	gint64 getFileLastModified (const std::string& filePath);
public:
	/**
	 * @brief constructor
	 */
	FileSystemScanner();
	/**
	 * @brief sets observers
	 * @param observer
	 */
	void addObserver(FileSystemScannerObserver* observer);
	/*TODO remove observer */
	//void removeObserver( FileSystemScannerObserver* observer );
	/**
	 * @brief starts scanning folder
	 * this function will return immediately as all processing is made in separate thread
	 * @see setOnFileFoundHandler and setOnFolderScanTerminated that will set callbacks
	 * for file found and scan terminate respectively
	 * @param folderName - folder to be used as root for filesystem scan
	 * @param recursive  - recursive
	 */
	void  startScanFolder( const std::string& folderName);
	/**
	 * @brief cancel the filesystem scan
	 * cancel the background thread and waits for thread join
	 * so after cancelScan() retunrs, no callbacks will be called
	 */
	void cancelScan();

};



#endif /* FILESYSTEM_SCANNER_H_ */
