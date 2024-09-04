#include <endian.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <curses.h>

#include "../include/id3reader.h"

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
    char choice;
    int i, chc2;
    int flag;
    printf("Available information:\n");
    printf("\t1:ID3 header information\n");
    printf("\t2:Size of ID3 tag\n");
    printf("\t3:No of ID3 frames\n");
    printf("\t4:ID3 frame list\n");
    printf("\t5:ID3 frame header information\n");
    printf("\t6:Details of ID3 frames\n");
    printf("\th:Help\n");

    flag = 1;
    while(flag) {
        scanf("%c",&choice);
        switch(choice) {
            case '1':
                show_id3tagheader(tag);
                break;
            case '2':
                printf("%d/%02x\n",tag->size,tag->size);
                break;
            case '3':
                printf("%d\n", tag->frame_no);
                break;
            case '4':
                for(i=0;i<tag->frame_no;i++)
                    printf("%d:%s\n",i,tag->frame_list[i]);
                break;
            case '5':
                printf("Enter frame number:");
                scanf("%d",&chc2);
                if(chc2<tag->frame_no)
                    show_id3frameheader(tag->frame_arr[chc2]->fhdr);
                else
                    printf("Out of range\n");
                break;
            case '6':
                printf("Enter frame number:");
                scanf("%d",&chc2);
                if(chc2<tag->frame_no)
                    printf("%s\n",tag->frame_arr[chc2]->data);
                else
                    printf("Out of range\n");
                break;
            case 'h':
                printf("Available information:\n");
                printf("\t1:ID3 header information\n");
                printf("\t2:Size of ID3 tag\n");
                printf("\t3:No of ID3 frames\n");
                printf("\t4:ID3 frame list\n");
                printf("\t5:ID3 frame header information\n");
                printf("\t6:Details of ID3 frames\n");
                break;

            default:
                flag = 0;
                break;
        }
        getchar();
    }
    /* Add driver code here */


    /* clean up resources */
    ID3_FREE(tag);
    close(fd);

    return 0;
}
