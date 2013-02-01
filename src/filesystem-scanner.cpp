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

FileSystemScanner::FileSystemScanner( FileFunctionType onFileFound )
	: m_onFileFound( onFileFound )
{

}

int FileSystemScanner::startExctractFolderRecursively( const std::string& folderName )
{
		DIR *dp;
		struct dirent *dirp;
		struct stat   dir_stat;
		if((dp  = opendir(folderName.c_str())) == NULL)
		{
			std::cout << "could not open folder " << folderName << std::endl;
			return -1;
		}

		while ((dirp = readdir(dp)) != NULL)
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
				startExctractFolderRecursively( fileName );
			}
			else
			{
				if( S_ISREG( dir_stat.st_mode) )
				{
					m_onFileFound( fileName );
				}
			}
		}
		closedir(dp);
		return 0;
}


