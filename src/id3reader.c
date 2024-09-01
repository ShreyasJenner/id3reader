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
#include "../include/tag_reader.h"
#include "../include/frame_reader.h"
#include "../include/free.h"



int main(int argc, char **argv) {
    int fd, tmp;
    struct id3_tag *tag;
    struct frames *frame;

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

    int code;
    struct frames *itr;
    itr = tag->fms;
    while(1) {
        scanf("%d",&code);

        if(code==-1)
            break;

        itr = tag->fms;
        while(code--) {
            itr = itr->next_frame;
        }
        for(int i=0;i<itr->fhdr->frame_size;i++) {
            printf("%c", itr->data[i]);
        }
        printf("\n");
    }

    ID3_FREE(tag);
    close(fd);

    return 0;
}
