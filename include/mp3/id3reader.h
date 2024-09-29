#ifndef ID3READER_H
#define ID3READER_H

#include "id3_structs.h"

int id3_tagcheck(char *filename);

ID3TagHeader *get_id3tagheader(int fd, ID3TagHeader *hdr);

void show_id3tagheader(ID3Tag *tag);

ID3Tag *get_id3tag(char *filename);

void id3_View(char *filename);

#endif
