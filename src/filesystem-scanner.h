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


typedef boost::function<void(const std::string&)> FileFunctionType;
class FileSystemScanner
{
	FileFunctionType m_onFileFound;
public:
	FileSystemScanner(FileFunctionType callback );
	int startExctractFolderRecursively( const std::string& folderName );
};



#endif /* FILESYSTEM_SCANNER_H_ */
