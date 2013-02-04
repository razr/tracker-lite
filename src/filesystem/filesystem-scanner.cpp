/*
 * filesystem-scanner.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */


#include "filesystem-scanner.h"
#include "file-metadata.h"




#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>

#define WITH_LOGGING
#ifdef WITH_LOGGING
#include <iostream>
#endif

#define WITH_STATISTICS
#ifdef WITH_STATISTICS
#include "statistics.h"
#endif

FileSystemScanner::FileSystemScanner()
{
	b_runThread = false;
}

void FileSystemScanner::setOnFileFoundHandler(FileFunctionType onFileFound)
{
	m_onFileFound = onFileFound;
}

void FileSystemScanner::setOnFolderScanTerminated( TerminateFunctionType onScanTerminated )
{
	m_onScanTerminated = onScanTerminated;
}

void * FileSystemScanner::scanThread( void* userData )
{
	FileSystemScanner* pInstance = static_cast<FileSystemScanner *>( userData );
	pInstance->scanFolder(pInstance->m_folderToScan);
	if( ! pInstance->m_onScanTerminated.empty() )
		pInstance->m_onScanTerminated();
	return NULL;
}

void FileSystemScanner::scanFolder( const std::string& folderName )
{
		DIR *dp;
		struct dirent *dirp;
		struct stat   dir_stat;

		if( ! b_runThread )
			return;

		if((dp  = opendir(folderName.c_str())) == NULL)
		{
			std::cout << "could not open folder " << folderName << std::endl;
			return;
		}

		while ( b_runThread && (dirp = readdir(dp)) != NULL)
		{
			std::string fileName = folderName + "/" + dirp->d_name;
			if( strcmp( dirp->d_name, "." ) == 0 )
				continue;
			if( strcmp( dirp->d_name, ".." ) == 0 )
				continue;

			if ( stat( fileName.c_str() , &dir_stat) == -1)
			{
				continue;
			}

			if( S_ISDIR(dir_stat.st_mode) )
			{
				scanFolder( fileName );
			}
			else
			{
				if( S_ISREG( dir_stat.st_mode) )
				{
#ifdef WITH_STATISTICS
					Statistics::getInstance().newFileScanned();
#endif
					if( ! m_onFileFound.empty() )
						m_onFileFound( fileName );
				}
			}
		}
		closedir(dp);

}

void FileSystemScanner::startScanFolderRecursively( const std::string& folderName )
{
		if( b_runThread )
			cancelScan();
		m_folderToScan = folderName;
	    if( pthread_create( &m_thread, NULL, scanThread, this) != 0 )
	    {
	    	b_runThread = false;
	    }
	    b_runThread = true;
}

void FileSystemScanner::cancelScan()
{
	if( b_runThread )
	{
		b_runThread = false;
		pthread_join( m_thread , NULL );
	}
}


