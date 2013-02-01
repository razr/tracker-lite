/*
 * taglib-metadata-extractor.h
 *
 *  Created on: Jan 30, 2013
 *      Author: ionut
 */

#ifndef TAGLIB_METADATA_EXTRACTOR_H_
#define TAGLIB_METADATA_EXTRACTOR_H_

#include "file-metadata-extractor.h"

class TagLibMetadataExtractor : public AbstractFileMetadataExtractor
{
public:
	virtual void extractMetadata( File& file ) const throw( ExtractMetadataError );
};

#endif /* TAGLIB_METADATA_EXTRACTOR_H_ */
