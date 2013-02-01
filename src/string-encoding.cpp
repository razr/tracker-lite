/*
 * string-encoding.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#include <string>
#include <algorithm>
#include "config.h"
#include "string-encoding.h"

std::string convertStringToUTF8( const std::string& str )
{
	return convertCStringToUTF8( str.c_str(), str.length() );
}

std::string convertCStringToUTF8( const char *str, size_t size )
{
	std::string retVal;
#ifdef HAVE_COUNTRY_CODING
#else
	retVal.assign(str, size);
#endif
	// TODO conversion
	return retVal;
}

