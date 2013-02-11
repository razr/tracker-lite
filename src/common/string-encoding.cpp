/*
 * string-encoding.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: ionut
 */

#include <string>
#include <algorithm>
#include <glib.h>
#include <unicode/ucsdet.h>
#include <cstring>

#include "config.h"
#include "string-encoding.h"
#include "logging.h"

#define HAVE_CONTRY_CODE
#ifdef  HAVE_CONTRY_CODE
// prioritized encodings assuming country is CHINA:
static const char* allowed_encodings[] =
{
		"GB18030",
		"Big5",
		"UTF-8",
		"UTF-16",
		"UTF-16BE",
		"UTF-16LE",
		"UTF-32",
		"UTF-32BE",
		"UTF-32LE"
};
static const int allowed_encodings_size = 9;
static const int default_fallback = 0;
#endif


gchar * encoding_guess_libicu (const gchar *buffer,
                             gsize        size,
                             int * confidence_out = NULL)
{
	gchar *encoding = NULL;
    UErrorCode status = U_ZERO_ERROR;
	UCharsetDetector* csd = ucsdet_open(&status);
	const UCharsetMatch *ucm;
	ucsdet_setText(csd, buffer, size, &status);
	ucm = ucsdet_detect(csd, &status);
	int32_t matchesFound;
	const UCharsetMatch ** allMatches = ucsdet_detectAll(csd,&matchesFound, &status);
	LOG(LOG_VERBOSE, "found %d possible matches", matchesFound );
	for( int32_t i = 0; i < matchesFound; ++ i)
	{
		const char* name = ucsdet_getName(allMatches[i], &status);
		encoding = g_strdup(name);
		int32_t confidence = ucsdet_getConfidence(allMatches[i], &status);
		LOG(LOG_VERBOSE, "match %s with confidence %d", encoding, confidence);
	}

	if( ucm != NULL )
	{
		const char* name = ucsdet_getName(ucm, &status);
		encoding = g_strdup(name);
		ucsdet_enableInputFilter(csd, true);
		int32_t confidence = ucsdet_getConfidence(ucm, &status);
		if( confidence_out )
		   *confidence_out = confidence;

		if (encoding)
			LOG(LOG_VERBOSE, "charset guess : %s with confidence %d", encoding, confidence );
		ucsdet_close(csd);
		return encoding;
	}

	ucsdet_close(csd);
	return NULL;
}


static gchar * convert_to_encoding(const gchar  *str,
                     gssize        len,
                     const gchar  *to_codeset,
                     const gchar  *from_codeset,
                     gsize        *bytes_read,
                     gsize        *bytes_written,
                     GError      **error_out)
{
    GError *error = NULL;
    gchar *word;
    LOG(LOG_VERBOSE, "converting from % to %s", from_codeset, to_codeset  );

    word = g_convert (str,
                      len,
                      to_codeset,
                      from_codeset,
                      bytes_read,
                      bytes_written,
                      &error);

    if (error!=NULL)
    {
    	LOG(LOG_ERROR, "convert error : %s", error->message );
    }
    else
    	LOG(LOG_VERBOSE, "convert done : %s", word );

    return word;
}
std::string convertStringToUTF8( const std::string& str )
{
//	LOG(LOG_VERBOSE, "str = %s", str.c_str()  );
	return convertCStringToUTF8( str.c_str(), str.length() );
}

std::string convertCStringToUTF8( const char *str, size_t size )
{

	std::string retVal;
	int confidence;
	char* trusted_encoding = encoding_guess_libicu( str, size, &confidence);
	if( trusted_encoding && confidence > 50 )
	{
	  LOG( LOG_VERBOSE, "trust encoding %s", trusted_encoding );
	  char* guessEncoding = convert_to_encoding(
												  str,
												  size,
												  "UTF-8",
												  trusted_encoding,
												  NULL, NULL, NULL);

		if( guessEncoding )
		{
			LOG(LOG_VERBOSE, "converted as : %s", guessEncoding );
			return guessEncoding;
		}
	}
#ifdef HAVE_CONTRY_CODE
	// if we know the country and icu can't guess, we can try decode with
	// some prioritized encodings, that must be defined for each country
	else
	{
		for( int i=0; i< allowed_encodings_size ; ++i)
			    {
					LOG(LOG_VERBOSE, "trying to decode with : %s ", allowed_encodings[i] );
					char* prioEncoding = convert_to_encoding(
															  str,
															  size,
															  "UTF-8",
															  allowed_encodings[i],
															  NULL, NULL, NULL);
					if( prioEncoding )
					{
						LOG(LOG_VERBOSE, "converted ad : %s", prioEncoding );
						return prioEncoding;
					}
			    }
	}
#endif

	// cannot decode, raw copy
	retVal.assign( str, size);
	return retVal;
}

