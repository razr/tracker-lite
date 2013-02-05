/*
 * string-encoding.h
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut.neicu@windriver.com
 */

#ifndef STRING_ENCODING_H_
#define STRING_ENCODING_H_


#include <string>
/**
 * @brief converts std::string to UTF-8 guessing the encoding or with country-specific priority table
 * @param string the string to be converted
 * @return UTF-8 string converted if succeded, or the original string if conversion was not possible
 */
std::string convertStringToUTF8( const std::string& string );
/**
 * @brief converts C strint to UTF-8 guessing the encoding or with country-specific priority table
 * @param string the string to be converted
 * @return UTF-8 string converted if succeded, or the original string if conversion was not possible
 */
std::string convertCStringToUTF8( const char *str, size_t size );

#endif /* STRING_ENCODING_H_ */
