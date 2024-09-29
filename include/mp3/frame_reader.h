#ifndef FRAME_READER_H
#define FRAME_READER_H

#include "id3_structs.h"
#include "stdheader.h"

int id3_framecheck(int fd);

void show_id3frameheader(ID3FrameHeader *hdr);

ID3FrameHeader *get_id3frameheader(int fd);

int get_id3framecount(int fd);

char **get_id3framelist(int fd, int count);

ID3Frame *get_id3frame(int fd);

#endif
