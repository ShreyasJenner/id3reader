#ifndef FRAME_READER_H
#define FRAME_READER_H

void show_id3frameheader(int fd);

struct frame_header *get_id3frameheader(int fd);

struct frames *get_id3frame(int fd);

#endif
