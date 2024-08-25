#include <endian.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <curses.h>

#include "../headers/struct.h"
#include "../headers/syncint.h"
#include "../headers/jpeg_writer.h"

#define BYTES_READ(bytes_read) printf("\tBYTES READ:%d\n",bytes_read);

/*
 * Function to print character n times
 */
void print_repeat(int count, char c) {
    int i;
    for(i=0;i<count;i++) {
        printf("%c",c);
    }
}

/*
 * Function checks if the string passed is equal to ID3
 */
int id3_checker(char *identifier) {
    return strncmp(identifier,"ID3", 3);
}

/*
 * Function checks if id3 tag exists in the first 10 bytes and then
 * gets the version, flags and tag size
 */
int tag_header(int fd, struct id3v2_header *header) {
    char identifier[3], version[2];
    uint8_t flags;
    uint32_t size;
    int bytes_read;

    // read first 3 bytes
    BYTES_READ((int)read(fd, identifier, 3));
    
    // check if id3 tag
    if(id3_checker(identifier)) {
        printf("No IDF tag\n");
        return 1;
    }
    printf("Identifier: IDF\n");

    // get version bytes
    BYTES_READ((int)read(fd, version, 2));
    printf("Version:%02x %02x\n",version[0],version[1]);

    // read flag byte
    BYTES_READ((int)read(fd, &flags, 1));
    
    // store flag bits in the header_flags struct
    header->header_flags.a = (flags&0x80)>>7;
    header->header_flags.b = (flags&0x40)>>6;
    header->header_flags.c = (flags&0x20)>>6;
    header->header_flags.d = (flags&0x10)>>5;

    printf("Flags:%d %d %d %d\n",header->header_flags.a,header->header_flags.b,header->header_flags.c,header->header_flags.d);

    // read tag size 4 bytes
    BYTES_READ((int)read(fd, &size, 4));

    // convert the data to big endian format
    size = __bswap_constant_32(size);
    header->tag_size = sync_safe_int_to_int(size);
    printf("Tag Size: 0x%08x / %d\n", header->tag_size, header->tag_size);

    return 0;
}

/*
 * Function reads tag data from the frames and displays it
 */
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
        BYTES_READ((int)read(fd, identifier, 4));

        // check if the 4 bytes are a frame identifier
        for(ident_itr=0;ident_itr<4;ident_itr++) {
            // identifier can only have characters A-Z or 0-9
            if(!(identifier[ident_itr]>='A' && identifier[ident_itr]<='Z') && !(identifier[ident_itr]>='0' && identifier[ident_itr]<='9')) {
                return ;
            }
        }

        // print formatting for frame data 
        print_repeat(max_x/2, '#');
        printf("\n");
        printf("%*s", max_x/8, "");
        printf("FRAME %d DATA\n", itr); 
        print_repeat(max_x/2, '#');
        printf("\n");

        printf("Frame ID:%c%c%c%c\n",identifier[0],identifier[1],identifier[2],identifier[3]);

        // read frame size bytes
        BYTES_READ((int)read(fd, &frame_size, 4));

        // convert data to big endian
        frame_size = __bswap_constant_32(frame_size);
        frame_size = sync_safe_int_to_int(frame_size);
        printf("Frame Size: 0x%08x / %d\n", frame_size, frame_size);
        
        // read flag bytes
        BYTES_READ((int)read(fd, &flags, 2));
        printf("Flags: 0x%04x\n",flags);

        // check if image data is next
        if(!strncmp(identifier, "APIC", 4)) {
            jpeg_writer(fd, frame_size, "test.jpeg");
        } else {    // print tag data normally
            // set file_pos to 0 
            file_pos = 0;
            //lseek(fd, header->tag_size+10, SEEK_SET);

            // read frame_size number of bytes and print it
            while(file_pos++<frame_size) {
                //BYTES_READ((int)read(fd, &data, 1));
                // read 25 bytes of frame data
                read(fd, &data, 1);
                printf("%c ", data);
            }
            printf("\n");
        }
    }
}

int main(int argc, char **argv) {
    int fd, max_y, max_x;
    struct id3v2_header header;

    // check if no argument have been passed
    if(argc<2) {
        printf("Missing file name argument\n");
        exit(1);
    }

    // check if too many args have been passed
    if(argc>2) {
        printf("Too many arguments\n");
        exit(1);
    }

    // Open the mp3 file
    fd = open(argv[1], O_RDONLY, 0);

    // Check if the file is available
    if(fd == -1) {
        printf("File not available\n");
        exit(1);
    }

    // Get screen width for nice printing
    initscr();
    getmaxyx(stdscr, max_y, max_x);
    endwin();
  
    // print heading for tag details 
    print_repeat(max_x, '#');
    printf("%*s\n",max_x/2,"TAG READ");
    print_repeat(max_x, '#');

    // get tag details if it is id3
    if(tag_header(fd, &header)) {
        return 0;
    }

    // if the tag is id3 then get the frame data
    print_repeat(max_x, '#');
    printf("%*s\n", max_x/2,"FRAME READ");
    print_repeat(max_x, '#');
    frame_read(fd, &header, max_x);

    close(fd);

    return 0;
}
