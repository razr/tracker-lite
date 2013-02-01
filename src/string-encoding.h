/*
 * string-encoding.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#ifndef STRING_ENCODING_H_
#define STRING_ENCODING_H_


#include <string>

std::string convertStringToUTF8( const std::string& string );
std::string convertCStringToUTF8( const char *str, size_t size );

#endif /* STRING_ENCODING_H_ */
