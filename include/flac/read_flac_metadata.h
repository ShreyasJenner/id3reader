#ifndef READ_FLAC_METADATA
#define READ_FLAC_METADATA

#define FLAC_IMAGE "/tmp/flacimage.img"

#include "flac/flac_structs.h"
#include "stdheader.h"

int flac_check(char *filename);

FLACMetadata *allocate_FLACMetadataSpace();

void store_FLACMetadata(FLACMetadata *metadata, FLAC__MetadataType metatype,
                        FLAC__StreamMetadata *streammetadata);

void view_FLACMetadata(char *filename);

FLACMetadata *get_FLACMetadata(char *filename);

void clean_FLACMetadata(FLACMetadata *metadata);

#endif // #ifndef READ_FLAC_METADATA
