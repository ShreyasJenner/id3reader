#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

struct id3v2_flags {
    uint8_t a;                  // Unsynchronization flag
    uint8_t b;                  // Extended Header
    uint8_t c;                  // Experimental indicator
    uint8_t d;                  // Footer present
};

struct id3v2_header {
    uint32_t tag_size;          // size of tag
    struct id3v2_flags header_flags;
};

struct id3v2_frame_header {
    int a;
    int b;
    int c;
    int h;
    int k;
    int m;
    int n;
    int p;
};
#endif
