#include "jpeg_writer.h"
#include "mp3/frame_reader.h"
#include "mp3/id3_free.h"
#include "mp3/structs.h"
#include "mp3/tag_reader.h"
#include "stdheader.h"
#include "syncint.h"
#include <stdio.h>

/* Driver Tag */
int main(int argc, char **argv) {
  /* Declaration */
  int fd;
  struct id3_tag *tag;

  /* check if file arg passed */
  if (argc != 2) {
    fprintf(stderr, "Correct number of arguments not passed\n");
    exit(1);
  }

  /* Open file and get id3 tag data */
  fd = open(argv[1], O_RDONLY, 0);
  if (fd < 0) {
    fprintf(stderr, "File error\n");
    exit(1);
  }

  /* check if passed file is mp3 */
  if (id3_tagcheck(fd)) {
    tag = get_id3tag(fd);
    if (tag == NULL) {
      fprintf(stderr, "No id3 data\n");
      exit(1);
    }

    // NOTE: temporary loop to print the id3 data
    int choice, i, flag;
    // char buf[1024];
    // char *endptr;

    printf("Available information:\n");
    printf("\t1:ID3 header information\n");
    printf("\t2:Size of ID3 tag\n");
    printf("\t3:No of ID3 frames\n");
    printf("\t4:ID3 frame list\n");
    printf("\t5:ID3 frame header information\n");
    printf("\t6:Details of ID3 frames\n");
    printf("\th:Help\n");

    flag = 1;
    while (flag) {
      if (scanf("%d", &choice) != 1) {
        choice = fgetc(stdin);
      }

      // do {
      //  if (!fgets(buf, 1024, stdin)) {
      //    perror("Reading stdin");
      //    return -1;
      //  }

      //  choice = strtol(buf, &endptr, 10);
      //} while (errno == ERANGE || endptr == buf || (*endptr && *endptr !=
      //'\n'));

      // printf("Choice %d\n", choice);

      switch (choice) {
      case 'h':
        printf("Available information:\n");
        printf("\t1:ID3 header information\n");
        printf("\t2:Size of ID3 tag\n");
        printf("\t3:No of ID3 frames\n");
        printf("\t4:ID3 frame list\n");
        printf("\t5:ID3 frame header information\n");
        printf("\t6:Details of ID3 frames\n");
        break;

      case 'q':
        printf("Exiting Program\n");
        flag = 0;
        break;

      case 1:
        show_id3tagheader(tag);
        break;
      case 2:
        printf("%d/%02x\n", tag->size, tag->size);
        break;
      case 3:
        printf("%d\n", tag->frame_no);
        break;
      case 4:
        for (i = 0; i < tag->frame_no; i++)
          printf("%d:%s\n", i, tag->frame_list[i]);
        break;
      case 5:
        printf("Enter frame number:");
        if (scanf("%d", &choice) != 1) {
          choice = fgetc(stdin);
        }

        if (choice < tag->frame_no)
          show_id3frameheader(tag->frame_arr[choice]->fhdr);
        else
          printf("Out of range\n");
        break;
      case 6:
        printf("Enter frame number:");
        if (scanf("%d", &choice) != 1) {
          choice = fgetc(stdin);
        }
        if (choice < tag->frame_no) {
          printf("%s\n", tag->frame_arr[choice]->data);
          jpeg_writer(fd, tag->frame_arr[choice]->fhdr->frame_size,
                      ".help/output.jpeg");
        } else {
          printf("Out of range\n");
        }
        break;

      default:
        break;
      }
    }
    /* Add driver code here */

    /* clean up resources */
    ID3_FREE(tag);
  } else {
    printf("Unkown file format\n");
  }

  /* close file descriptor */
  close(fd);

  return 0;
}
