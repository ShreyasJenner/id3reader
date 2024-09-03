#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/syncint.h"
#include "../include/structs.h"
#include "../include/tag_reader.h"
#include "../include/frame_reader.h"

/*
 * Function is passed a file descriptor 
 * Checks if file contains id3 tag
 */
int id3_tagcheck(int fd) {
    char tag[3];

    read(fd, tag, 3);
    lseek(fd, -3, SEEK_CUR);

    return tag[0]=='I' && tag[1]=='D' && tag[2]=='3';
}

/*
 * Function gets id3 tag header info and stores in struct
 */
struct tag_header *get_id3tagheader(int fd, struct tag_header *hdr) {
    char identifier[3];
    uint8_t version[2];
    uint8_t flags;
    uint32_t size;


    /* allocate space for tag header */
    hdr = malloc(sizeof(struct tag_header));

    /* read data from id3 header */
    read(fd, identifier, 3);
    read(fd, version, 2);
    read(fd, &flags, 1);
    read(fd, &size, 4);

    /* change endianess of var `size` to big endian */
    size = __bswap_constant_32(size);

    /* fill struct */
    strcpy(hdr->identifier,identifier);
    hdr->major_ver = version[0];
    hdr->revision_no = version[1];
    hdr->flags[0] = (flags&8u)>>7;
    hdr->flags[1] = (flags&7u)>>6;
    hdr->flags[2] = (flags&6u)>>5;
    hdr->flags[3] = (flags&5u)>>4;
    hdr->size = sync_safe_int_to_int(size);

    return hdr;
}


/*
 * print the id3 tag header onto the terminal
 */
void show_id3tagheader(struct id3_tag *tag) {
    printf("Identifier: %s\n",tag->hdr->identifier);
    printf("Major Version: %d\n",tag->hdr->major_ver);
    printf("Revision No: %d\n",tag->hdr->revision_no);
    printf("Unsynchronization: %s\n",(tag->hdr->flags[0]&8u)>>7?"True":"False");
    printf("Extended Header: %s\n",(tag->hdr->flags[1]&7u)>>6?"True":"False");
    printf("Experimental Indicator: %s\n",(tag->hdr->flags[2]&6u)>>5?"True":"False");
    printf("Footer: %s\n",(tag->hdr->flags[3]&5u)>>4?"True":"False");
    printf("Size: %d/%02x\n",tag->hdr->size,tag->hdr->size);
}


/*
 * Function is passed a file descriptor pointing to an mp3 file
 * Return a struct containing all id3 information if id3 tag is present
 * Return NULL if no id3 tag
 * The function does not reposition the file descriptor after reading
*/
struct id3_tag *get_id3tag(int fd) {
    int err, i;
    struct id3_tag *tag;
    struct ret_multiple *mult;

    /* check for id3 tag */
    lseek(fd, 0, SEEK_SET);
    err = id3_tagcheck(fd);
    if(err == 0) {
        return NULL;
    }
    
    /* allocate space for tag struct */
    tag = malloc(sizeof(struct id3_tag));

    /* get tag header */
    tag->hdr = get_id3tagheader(fd, tag->hdr);

    /* get number of frames and and list of frames */
    tag->frame_no = get_id3framecount(fd);
    tag->frame_list = get_id3framelist(fd,tag->frame_no);
    tag->frame_arr = malloc(sizeof(struct frames *) * tag->frame_no);

    /* calculate tag size */
    tag->size = tag->hdr->size + 10 + (10*tag->hdr->flags[3]);

    /* get frames */
    for(i=0;i<tag->frame_no;i++)
        tag->frame_arr[i] = get_id3frame(fd);

    return tag;
}
