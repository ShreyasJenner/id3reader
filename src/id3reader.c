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
#include "../include/tag_reader.h"
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

    /* Add driver code here */
    /* Add driver code here */


    /* clean up resources */
    ID3_FREE(tag);
    close(fd);

    return 0;
}
