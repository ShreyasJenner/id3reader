#include <endian.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>

#include "../headers/struct.h"
#include "../headers/syncint.h"

#define BYTES_READ(bytes_read) printf("\tBYTES READ:%d\n",bytes_read);

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
 * Function reads tag data from the frames
 */
void frame_read(int fd, struct id3v2_header* header) {
    char identifier[4], data;
    uint32_t frame_size;
    uint16_t flags;
    int itr, file_pos;

    // read identifier bytes
    BYTES_READ((int)read(fd, identifier, 4));
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

    // set file_pos to current fp position
    file_pos = 21;
    itr = 0;
//    lseek(fd, 126636+20, 0);
    while(file_pos++ < 46+(25*1)) {
        //BYTES_READ((int)read(fd, &data, 1));
        // read 25 bytes of frame data
        read(fd, &data, 1);
        printf("%c", data);

        if(itr++==24) {
            printf("\n");
            itr = 0;
        }
    }
}

int main() {
    int fd;
    struct id3v2_header header;

    fd = open("sample-files/sample2.mp3", O_RDONLY, 0);

    if(fd == -1) {
        printf("File not available\n");
        exit(1);
    }
   
    // get tag details if it is id3
    printf("TAG READ\n");
    if(tag_header(fd, &header)) {
        return 0;
    }

    // if the tag is id3 then get the frame data
    printf("\nFRAME READ\n");
    frame_read(fd, &header);

    close(fd);

    return 0;
}
