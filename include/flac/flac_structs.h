#ifndef FLAC_STRUCTS_H
#define FLAC_STRUCTS_H

// global
#include "stdheader.h"

typedef struct flac_metadata FLACMetadata;

/* stream info metadata block */
struct flac_metadata {
  FLAC__StreamMetadata *streaminfo;
  FLAC__StreamMetadata *padding;
  FLAC__StreamMetadata *app;
  FLAC__StreamMetadata *seektable;
  FLAC__StreamMetadata *vorbis_comment;
  FLAC__StreamMetadata *cue_sheet;
  FLAC__StreamMetadata *picture;
};

#endif // !FLAC_STRUCTS_H
