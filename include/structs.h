#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>


/*
 * frame_id: stores id of frame
 * size: size of data in frame
 * flags: store status of flags
 */
struct frame_header {
    char frame_id[4];
    int frame_size;

    /*
     * 0: tag alter preservation
     * 1: frame alter preservation
     * 2: read only
     * 3: group frame
     * 4: compression
     * 5: encryption
     * 6: uncsynchronization
     * 7: data length indicator
     */
    bool flags[8];
};

/*
 * fhdr: stores pointer to struct frame_header
 * data: pointer to data stored in frame
 * next_frame: pointer to next id3 frame
 */
struct frames {
    struct frame_header *fhdr;

    uint8_t *data;

    struct frames *next_frame;
};

/*
 * identifier: stores first three bytes of id3 tag
 * major_ver: stores major version of id3
 * revision_no: stores revision no of id3
 * flags: indicates status of certain flags
 * size: stores size of id3 tag - header
 */
struct tag_header {
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
 * frame_no: stores number of frames
 */
struct id3_tag {
    /* normal data */
    struct tag_header *hdr;
    struct frames *fms; 

    /* derived data */
    uint32_t size;
    int frame_no;
};

#endif
