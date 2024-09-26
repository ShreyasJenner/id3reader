#ifndef FRAME_READER_H
#define FRAME_READER_H

#include "id3_structs.h"
#include "stdheader.h"

int id3_framecheck(int fd);

void show_id3frameheader(struct frame_header *fhdr);

struct frame_header *get_id3frameheader(int fd);

int get_id3framecount(int fd);

char **get_id3framelist(int fd, int count);

struct frames *get_id3frame(int fd);

#endif
