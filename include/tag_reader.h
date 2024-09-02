#ifndef TAG_READER_H
#define TAG_READER_H

#include "../include/structs.h"

int id3_tagcheck(int fd);

struct tag_header *get_id3tagheader(int fd, struct tag_header *hdr);

void show_id3tagheader(int fd);

struct id3_tag *get_id3tag(int fd);

#endif
