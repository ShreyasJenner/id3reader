#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

/*
 * identifier: stores first three bytes of id3 tag
 * major_ver: stores major version of id3
 * revision_no: stores revision no of id3
 * flags: indicates status of certain flags
 * size: stores size of id3 tag - header
 */
struct header {
    char identifier[3];
    uint8_t major_ver;
    uint8_t revision_no;

    /*
     * 0 - uncsynchronization
     * 1 - extended header
     * 2 - experimental indicator
     * 3 - footer present
     */
    bool flags[4];
    uint32_t size;
};


/*
 * hdr: pointer to struct header
 * fms: pointer to struct frames
 * size: stores size of entire id3 tag including header
 */
struct id3_tag {
    /* normal data */
    struct header *hdr;
    struct frames *fms; 

    /* derived data */
    uint32_t size;
};

#endif
