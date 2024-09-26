#include "mp3/id3reader.h"
#include "jpeg_writer.h"
#include "mp3/frame_reader.h"
#include "mp3/id3_free.h"
#include "mp3/id3_structs.h"
#include "stdheader.h"
#include "syncint.h"

/*
 * Function is passed a file descriptor
 * Checks if file contains id3 tag
 * Restores position of file pointer after reading
 */
int id3_tagcheck(char *filename) {
  /* Declaration */
  int fd;
  char tag[3];

  /* Open file */
  fd = open(filename, O_RDONLY);

  /* read file */
  read(fd, tag, 3);
  lseek(fd, -3, SEEK_CUR);

  /* close file */
  close(fd);

  return tag[0] == 'I' && tag[1] == 'D' && tag[2] == '3';
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
  strcpy(hdr->identifier, identifier);
  hdr->major_ver = version[0];
  hdr->revision_no = version[1];
  hdr->flags[0] = (flags & 8u) >> 7;
  hdr->flags[1] = (flags & 7u) >> 6;
  hdr->flags[2] = (flags & 6u) >> 5;
  hdr->flags[3] = (flags & 5u) >> 4;
  hdr->size = sync_safe_int_to_int(size);

  return hdr;
}

/*
 * print the id3 tag header onto the terminal
 */
void show_id3tagheader(struct id3_tag *tag) {
  printf("Identifier: %s\n", tag->hdr->identifier);
  printf("Major Version: %d\n", tag->hdr->major_ver);
  printf("Revision No: %d\n", tag->hdr->revision_no);
  printf("Unsynchronization: %s\n",
         (tag->hdr->flags[0] & 8u) >> 7 ? "True" : "False");
  printf("Extended Header: %s\n",
         (tag->hdr->flags[1] & 7u) >> 6 ? "True" : "False");
  printf("Experimental Indicator: %s\n",
         (tag->hdr->flags[2] & 6u) >> 5 ? "True" : "False");
  printf("Footer: %s\n", (tag->hdr->flags[3] & 5u) >> 4 ? "True" : "False");
  printf("Size: %d/%02x\n", tag->hdr->size, tag->hdr->size);
}

/*
 * Function is passed a file descriptor pointing to an mp3 file
 * Return a struct containing all id3 information if id3 tag is present
 * Return NULL if no id3 tag
 * The function does not reposition the file descriptor after reading
 */
struct id3_tag *get_id3tag(char *filename) {
  int err, i, fd;
  struct id3_tag *tag;

  /* check for id3 tag */
  err = id3_tagcheck(filename);
  if (err == 0) {
    return NULL;
  }

  /* open file */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening file\n");
    return NULL;
  }

  /* allocate space for tag struct */
  tag = malloc(sizeof(struct id3_tag));

  /* get tag header */
  tag->hdr = get_id3tagheader(fd, tag->hdr);

  /* get number of frames and and list of frames */
  tag->frame_no = get_id3framecount(fd);
  tag->frame_list = get_id3framelist(fd, tag->frame_no);
  tag->frame_arr = malloc(sizeof(struct frames *) * tag->frame_no);

  /* calculate tag size */
  tag->size = tag->hdr->size + 10 + (10 * tag->hdr->flags[3]);

  /* get frames */
  for (i = 0; i < tag->frame_no; i++)
    tag->frame_arr[i] = get_id3frame(fd);

  /* reposition file descriptor */
  lseek(fd, 0, SEEK_SET);

  return tag;
}

/* Function to view id3 data */
void id3_View(char *filename) {
  /* Declaration */
  int choice, i, flag;
  struct id3_tag *tag;

  /* get the id3 tag and store it */
  tag = get_id3tag(filename);
  if (tag == NULL) {
    fprintf(stderr, "No id3 data\n");
    exit(1);
  }

  flag = 1;
  choice = 'h';
  while (flag) {
    switch (choice) {
    case 'h':
      printf("Available information:\n");
      printf("\t1:ID3 header information\n");
      printf("\t2:Size of ID3 tag\n");
      printf("\t3:No of ID3 frames\n");
      printf("\t4:ID3 frame list\n");
      printf("\t5:ID3 frame header information\n");
      printf("\t6:Details of ID3 frames\n");
      printf("\th:Help\n");
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
        /* check if current tag stores jpeg data */
        if (!strcmp(tag->frame_arr[choice]->fhdr->frame_id, "APIC")) {
          jpeg_writer(filename, tag->frame_arr[choice]->fhdr->frame_size,
                      ".help/output.jpeg");
        } else {
          printf("%s\n", tag->frame_arr[choice]->data);
        }
      } else {
        printf("Out of range\n");
      }
      break;

    default:
      break;
    }

    if (flag && scanf("%d", &choice) != 1) {
      choice = fgetc(stdin);
    }
  }

  /* clean up resources */
  ID3_FREE(tag);
}
