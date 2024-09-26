#ifndef ID3READER_H
#define ID3READER_H

#include "id3_structs.h"

int id3_tagcheck(char *filename);

struct tag_header *get_id3tagheader(int fd, struct tag_header *hdr);

void show_id3tagheader(struct id3_tag *tag);

struct id3_tag *get_id3tag(char *filename);

void id3_View(char *filename);

#endif
