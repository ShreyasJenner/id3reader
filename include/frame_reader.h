#ifndef FRAME_READER_H
#define FRAME_READER_H

int id3_framecheck(int fd);

void show_id3frameheader(int fd);

struct frame_header *get_id3frameheader(int fd);

int get_id3framecount(int fd);

struct frames *get_id3frame(int fd);

#endif
