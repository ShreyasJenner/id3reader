#ifndef MP3_LUT_H
#define MP3_LUT_H

#include "stdheader.h"
#include "structs.h"

int verify_mp3Header(uint8_t *bytes);

void show_mp3FrameHeader(uint8_t *bytes);

int get_mp3FrameHeader(uint8_t *bytes, struct mp3_frame_header_data *mfhd);

#endif
