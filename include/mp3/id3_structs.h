#ifndef STRUCT_H
#define STRUCT_H

#include "stdheader.h"

typedef struct id3_tag ID3Tag;
typedef struct tag_header ID3TagHeader;
typedef struct frames ID3Frame;
typedef struct frame_header ID3FrameHeader;
typedef struct mp3_frame_header_data MP3FrameHeader;

/*
 * Struct to store mp3 frame header data
 * Refer mp3_lut.c file to get a list of all the flag details
 * OR
 * Refer http://mpgedit.org/mpgedit/mpeg_format/mpeghdr.htm
 *
 * v_id: mpeg version
 * layer: mp3 layer
 * crc: is crc there
 * bitrate: bitrate of mp3 file in Kbs
 * samplerate: samplerate frequency in hz
 * padding: is padding there
 * channel: channel mode used
 * mode_extension: [refer mp3 docs]
 * copyright: is copyright there
 * original: is audio original
 * emphasis: [refer mp3 docs]
 *
 * frame_size: number of samples contained in frame [depends on layer]
 * frame_length: length of frame in bytes [depends on layer]
 * channel_no: number of channes [depends on channel mode]
 *
 */
struct mp3_frame_header_data {
  // normal data
  float v_id;
  int layer;
  bool crc;
  int bitrate;
  int samplerate;
  bool padding;
  int priv;
  char channel[14];
  int mode_extension;
  bool copyright;
  bool original;
  char emphasis[10];

  // derived data
  int frame_size;
  int frame_length;
  int channel_no;
};

/*
 * frame_id: stores id of frame
 *           id is always a 4 character string
 * frame_size: size of data in frame
 * flags[8]: store status of flags
 */
struct frame_header {
  char frame_id[5];
  int frame_size;

  /*
   * 0: tag alter preservation
   * 1: frame alter preservation
   * 2: read only
   * 3: group frame
   * 4: compression
   * 5: encryption
   * 6: uncsynchronization
   * 7: data length indicator
   */
  bool flags[8];
};

/*
 * *fhdr: stores pointer to struct frame_header
 * *data: pointer to data stored in frame
 */
struct frames {
  ID3FrameHeader *hdr;

  uint8_t *data;
};

/*
 * identifier[3]: stores first three bytes of id3 tag
 * version[2]: stores version of id3
 *            version[0] -> major version
 *            version[1] -> revision number
 * flags[4]: indicates status of certain flags
 * size: stores size of id3 tag - header
 */
struct tag_header {
  char identifier[3];
  uint8_t version[2];

  /*
   * 0 - uncsynchronization
   * 1 - extended header
   * 2 - experimental indicator
   * 3 - footer present
   */
  bool flags[4];
  uint32_t size;
};

/*
 * *hdr: pointer to struct header
 * **frame_arr: pointer to array of struct frames
 * size: stores size of entire id3 tag including header
 * frame_no: stores number of frames
 * **frame_list: stores ids of all frames
 */
struct id3_tag {
  /* normal data */
  ID3TagHeader *hdr;
  ID3Frame **frame_arr;

  /* derived data */
  uint32_t size;
  int frame_no;
  char **frame_list;
};

#endif
