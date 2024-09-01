/*
 * Function reads tag data from the frames and displays it
void frame_read(int fd, struct id3v2_header* header, int max_x) {
    char identifier[4];
    uint32_t frame_size;
    uint16_t flags;
    uint8_t data;
    int itr, file_pos, ident_itr, image_fd;
    
    itr = 1;

    // read all existing frames
    while(itr++) {
        // read identifier bytes

        // check if the 4 bytes are a frame identifier
        for(ident_itr=0;ident_itr<4;ident_itr++) {
            // identifier can only have characters A-Z or 0-9
            if(!(identifier[ident_itr]>='A' && identifier[ident_itr]<='Z') && !(identifier[ident_itr]>='0' && identifier[ident_itr]<='9')) {
                return ;
            }
        }

        // print formatting for frame data 
        printf("\n");
        printf("%*s", max_x/8, "");
        printf("FRAME %d DATA\n", itr); 
        printf("\n");

        printf("Frame ID:%c%c%c%c\n",identifier[0],identifier[1],identifier[2],identifier[3]);

        // read frame size bytes

        // convert data to big endian
        frame_size = __bswap_constant_32(frame_size);
        frame_size = sync_safe_int_to_int(frame_size);
        printf("Frame Size: 0x%08x / %d\n", frame_size, frame_size);
        
        // read flag bytes
        printf("Flags: 0x%04x\n",flags);

        // check if image data is next
        if(!strncmp(identifier, "APIC", 4)) {
            jpeg_writer(fd, frame_size, "data/test.jpeg");
        } else {    // print tag data normally
            // set file_pos to 0 
            file_pos = 0;
            //lseek(fd, header->tag_size+10, SEEK_SET);

            // read frame_size number of bytes and print it
            while(file_pos++<frame_size) {
                // read 25 bytes of frame data
                read(fd, &data, 1);
                printf("%c ", data);
            }
            printf("\n");
        }
    }
}
 */

#include <endian.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <curses.h>

#include "../include/structs.h"
#include "../include/syncint.h"
#include "../include/jpeg_writer.h"


/*
 * Function is passed a file descriptor 
 * Checks if file contains id3 tag
 */
int id3_check(int fd) {
    char tag[3];

    read(fd, tag, 3);
    lseek(fd, -3, SEEK_CUR);

    return tag[0]=='I' && tag[1]=='D' && tag[2]=='3';
}

/*
 * Function gets id3 tag header info and stores in struct
 */
struct header *get_id3tagheader(int fd, struct header *hdr) {
    char identifier[3];
    uint8_t version[2];
    uint8_t flags;
    uint32_t size;


    /* allocate space for tag header */
    hdr = malloc(sizeof(struct header));

    /* read data from id3 header */
    read(fd, identifier, 3);
    read(fd, version, 2);
    read(fd, &flags, 1);
    read(fd, &size, 4);

    /* change endianess of number */
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
void show_id3tagheader(int fd) {
    lseek(fd, 0, SEEK_SET);

    char identifier[3];
    uint8_t version[2];
    uint8_t flags;
    uint32_t size;


    /* read data from id3 header */
    read(fd, identifier, 3);
    read(fd, version, 2);
    read(fd, &flags, 1);
    read(fd, &size, 4);

    /*
     * size is stored in as sync-safe int in big endian but read as 
     * little endian due to system specifications
     * convert to big endian and convert from sync-safe int to int
     * size of tag does not include header so add 10 to the size
     * size of header is 10 bytes
     */
    size = __bswap_constant_32(size);
    size = sync_safe_int_to_int(size) + 10;
    
    
    /* print the info */
    printf("Identifier: %s\n",identifier);
    printf("Major Version: %d\n",version[0]);
    printf("Revision No: %d\n",version[1]);
    printf("Unsynchronization: %s\n",(flags&8u)>>7?"True":"False");
    printf("Extended Header: %s\n",(flags&7u)>>6?"True":"False");
    printf("Experimental Indicator: %s\n",(flags&6u)>>5?"True":"False");
    printf("Footer: %s\n",(flags&5u)>>4?"True":"False");
    printf("Size: %d/%02x\n",size,size);
}


/*
 * Function is passed a file descriptor pointing to an mp3 file
 * It reads the file and checks if an id3 tag is present
 * Return a struct containing all id3 information if id3 tag
 * Return NULL if no id3 tag
 * The function does not reposition the file descriptor so file descriptor 
 * is still located at whatever position was read to
*/
struct id3_tag *get_id3tag(int fd) {
    int err;
    struct id3_tag *tag;

    /* check for id3 tag */
    lseek(fd, 0, SEEK_SET);
    err = id3_check(fd);
    if(err == 0) {
        return NULL;
    }
    
    /* allocate space for tag */
    tag = malloc(sizeof(struct id3_tag));

    /* get tag header */
    tag->hdr = get_id3tagheader(fd, tag->hdr);


    /* store tag data */
    tag->size = tag->hdr->size + 10 + (10*tag->hdr->flags[3]);


    return tag;
}



int main(int argc, char **argv) {
    int fd, tmp;
    struct id3_tag *tag;

    if(argc<2) {
        printf("Missing file name argument\n");
        exit(1);
    }

    /* open file */
    fd = open(argv[1], O_RDONLY, 0);
    if(fd<0) {
        printf("File error\n");
        exit(1);
    }

    /* get id3 tag data */
    tag = get_id3tag(fd); 
    if(tag == NULL) {
        printf("No id3 data\n");
        exit(1);
    }

    /* print tag data */
    show_id3tagheader(fd);

    free(tag->hdr);
    free(tag);
    close(fd);

    return 0;
}
