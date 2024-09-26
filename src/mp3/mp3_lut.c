/*
 * Program stores static lookup tables for mp3 frames.
 * When passed an array of 4 bytes that contain the mp3 frame header, it reads
 * it and returns a struct that contains the mp3 frame information including
 * the size and number of samples in the frame
 */

#include "mp3/mp3_lut.h"
#include "mp3/id3_structs.h"
#include "stdheader.h"

/*
 * 00 - v2.5
 * 01 - reserved
 * 10 - v2
 * 11 - v1
 */
static const float mpeg_version_id[4] = {2.5, 0, 2, 1};

/*
 * 00 - reserved
 * 01 - 3
 * 10 - 2
 * 00 - 1
 */
static const int layer[4] = {0, 3, 2, 1};

/*
 * 0 - true
 * 1 - false
 */
static const bool crc_protection[2] = {true, false};

/*
 * 11 11 - v1, l1
 * 11 10 - v1, l2
 * 11 01 - v1, l3
 * 10 11 - v2, l1
 * 10 10 - v2, l2
 * 10 01 - v2, l3
 */
static const int bitrate_index[2][3][16] = {
    {{0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
     {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
     {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, -1}},
    {{0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1},
     {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, -1},
     {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,
      -1}}};

/*
 * 00 - 44.1Khz
 * 01 - 48Khz
 * 10 - 3.2Khz
 * 11 - reserved
 */
static const int sampling_rate[4][4] = {{},
                                        {11025, 12000, 8000, 0},
                                        {22050, 24000, 16000, 0},
                                        {44100, 48000, 32000, 0}};

static const bool padding[2] = {false, true};

static const int private[2] = {-1, -1};

static const char *channel_mode[4] = {"stereo", "joint(stereo)", "dual(stereo)",
                                      "mono"};

static const int mode_extension[4] = {-1, -1, -1, -1};

static const bool copyright[2] = {false, true};

static const bool original[2] = {true, false};

static const char *emphasis[4] = {"none", "50/15 ms", "reserved", "CCIT J.17"};

/*
 * Function checks it the byte array passed contains an mp3 frame header
 * This is not an absolute guarantee that the header passed is one
 * as, method used to verify if it is an mp3 header is to check the first
 * 11 bits of the array, and see if they are one
 * Data can be fabricated to resemble an mp3 frame hedaer by setting the
 * 1st 11 bits to one, but may not contain any actual mp3 frame data
 */
int verify_mp3Header(uint8_t *bytes) {
  uint16_t mask;

  /* bit version: 1111 1111 1110 0000 */
  mask = 65504;

  /* verify if the 1st 11 bits are 1; number is stored as little endian on
   * system */
  return (((bytes[0] << 8) | bytes[1]) & mask) == mask;
}

/*
 * Function prints the mp3 frame header deatails onto the terminal when passed
 * a byte array containing the mp3 frame header
 */
void show_mp3FrameHeader(uint8_t *bytes) {
  /* verify if mp3 header exists */
  if (!verify_mp3Header(bytes)) {
    printf("invalid mp3 frame header\n");
    exit(1);
  }

  float v_id;
  int mp3_layer;
  bool crc;
  int bitrate;
  int samp_rate;
  bool pad;
  char *chan;
  bool copyrit;
  bool orig;
  char *emph;

  v_id = mpeg_version_id[(bytes[1] & 24u) >> 3];
  mp3_layer = layer[(bytes[1] & 6u) >> 1];
  crc = crc_protection[(bytes[1] & 1u) >> 0];
  bitrate = bitrate_index[((bytes[1] & 24u) >> 3) - 2]
                         [((bytes[1] & 6u) >> 1) - 1][(bytes[2] & 240u) >> 4];
  samp_rate = sampling_rate[(bytes[1] & 24u) >> 3][(bytes[2] & 12u) >> 2];
  pad = padding[(bytes[2] & 2u) >> 1];
  chan = (char *)channel_mode[(bytes[3] & 192u) >> 6];
  copyrit = copyright[(bytes[3] & 8u) >> 3];
  orig = original[(bytes[3] & 4u) >> 2];
  emph = (char *)emphasis[(bytes[3] & 2u) >> 0];

  printf("MPEG Version-%.1f\n", v_id);
  printf("Layer %d\n", mp3_layer);
  printf("CRC: %s\n", crc ? "True" : "False");
  printf("Bitrate: %dKbps\n", bitrate);
  printf("Sampling Rate: %dKHz\n", samp_rate);
  printf("Padding: %s\n", pad ? "True" : "False");
  printf("Channel: %s\n", chan);
  printf("Copyright: %s\n", copyrit ? "True" : "False");
  printf("Original Copy: %s\n", orig ? "True" : "False");
  printf("Emphasis: %s\n", emph);
}

/*
 * Function to print the mp3 frame details from the frame lookup
 * table
 * The function is passed an byte array of size 4 that contains
 * the header of an mp3 frame
 * It returns a struct containing info about the frame
 */
// int get_mp3FrameHeader(uint8_t *bytes, struct mp3_frame_header_data *mfhd) {
//   /* verify if mp3 header exists */
//   if (!verify_mp3Header(bytes)) {
//     printf("invalid mp3 frame header\n");
//     return -1;
//   }
//
//   /* Declaration */
//   float v_id;
//   int mp3_layer;
//   bool crc;
//   int bitrate;
//   int samp_rate;
//   bool pad;
//   char *chan;
//   bool copyrit;
//   bool orig;
//   char *emph;
//
//   /* get mp3 frame data from lookup tables */
//   v_id = mpeg_version_id[(bytes[1] & 24u) >> 3];
//   mp3_layer = layer[(bytes[1] & 6u) >> 1];
//   crc = crc_protection[(bytes[1] & 1u) >> 0];
//   bitrate = bitrate_index[((bytes[1] & 24u) >> 3) - 2]
//                          [((bytes[1] & 6u) >> 1) - 1][(bytes[2] & 240u) >>
//                          4];
//   samp_rate = sampling_rate[(bytes[1] & 24u) >> 3][(bytes[2] & 12u) >> 2];
//   pad = padding[(bytes[2] & 2u) >> 1];
//   chan = (char *)channel_mode[(bytes[3] & 192u) >> 6];
//   copyrit = copyright[(bytes[3] & 8u) >> 3];
//   orig = original[(bytes[3] & 4u) >> 2];
//   emph = (char *)emphasis[(bytes[3] & 2u) >> 0];
//
//   /* allocate space for struct and store data */
//   mfhd->v_id = v_id;
//   mfhd->layer = mp3_layer;
//   mfhd->crc = crc;
//   mfhd->bitrate = bitrate;
//   mfhd->samplerate = samp_rate;
//   mfhd->padding = pad;
//   strcpy(mfhd->channel, chan);
//   mfhd->copyright = copyrit;
//   mfhd->original = orig;
//   strcpy(mfhd->emphasis, emph);
//
//   /* store derived data */
//   if (mfhd->layer == 3 || mfhd->layer == 2) {
//     mfhd->frame_size = 1152;
//     mfhd->frame_length =
//         144 * mfhd->bitrate * 1000 / mfhd->samplerate + mfhd->padding;
//   } else if (mfhd->layer == 1) {
//     mfhd->frame_size = 384;
//     mfhd->frame_length =
//         (12 * mfhd->bitrate * 1000 / mfhd->samplerate + (4 * mfhd->padding))
//         * 4;
//   }
//
//   if (!strcmp(chan, "stereo") || !strcmp(chan, "joint(stereo)") ||
//       !strcmp(chan, "dual(stereo)"))
//     mfhd->channel_no = 2;
//   else
//     mfhd->channel_no = 1;
//
//   return 1;
// }
