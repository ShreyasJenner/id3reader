#include "mp3/frame_reader.h"
#include "mp3/structs.h"
#include "stdheader.h"
#include "syncint.h"

/*
 * Check if id3 frame header exists at file descriptor position
 * function resets fd back to initial position it was in when passed to this
 * function
 */
int id3_framecheck(int fd) {
  int i;
  char id[4];

  read(fd, id, 4);

  /* check if the 1st 4 bytes can be an id3 frame header id */
  for (i = 0; i < 4; i++) {
    if (!(id[i] >= 'A' && id[i] <= 'Z') && !(id[i] >= '0' && id[i] <= '9')) {
      lseek(fd, -4, SEEK_CUR);
      return 0;
    }
  }
  lseek(fd, -4, SEEK_CUR);
  return 1;
}

/*
 * Function to get number of frames in id3 tag
 * Assumes that fd is pointing to the start of the first frame
 * Sets fd to the position it was in when passed to the function
 */
int get_id3framecount(int fd) {
  int cntr, pos;
  struct frame_header *fhdr;

  /* Initialization */
  cntr = 0;
  pos = lseek(fd, 0, SEEK_CUR);

  /* loop will keep running until frames run out */
  while (true) {
    /* get id3 frame header
     * if it doesnt exist, then break the loop
     */
    fhdr = get_id3frameheader(fd);
    if (fhdr == NULL)
      break;

    /* set fd to end of frame and increment counter */
    lseek(fd, fhdr->frame_size, SEEK_CUR);
    cntr++;
  }

  /* set fd to original position */
  lseek(fd, pos, SEEK_SET);

  /* store cntr in struct and return struct ptr */
  return cntr;
}

/*
 * Function to get list of frames in id3 tag
 * Assumes that fd is pointing to the start of the first frame
 * Sets fd to the position it was in when passed to the function
 */
char **get_id3framelist(int fd, int count) {
  int cntr, pos;
  struct frame_header *fhdr;
  char **frame_list;

  /* Initialization */
  cntr = 0;
  pos = lseek(fd, 0, SEEK_CUR);
  frame_list = (char **)malloc(sizeof(char *) * count);

  /* loop will keep running until frames run out */
  while (true) {
    /* get id3 frame header
     * if it doesnt exist, then break the loop
     */
    fhdr = get_id3frameheader(fd);
    if (fhdr == NULL)
      break;

    /* allocate space for frame id and store */
    frame_list[cntr] = (char *)malloc(sizeof(char) * 4);
    strcpy(frame_list[cntr++], fhdr->frame_id);

    /* set fd to end of frame */
    lseek(fd, fhdr->frame_size, SEEK_CUR);
    free(fhdr);
  }

  /* set fd to original position */
  lseek(fd, pos, SEEK_SET);

  /* store ptr to */
  return frame_list;
}

/*
 * Function prints an id3 frame header information onto terminal
 */
void show_id3frameheader(struct frame_header *fhdr) {
  printf("Frame ID: %s\n", fhdr->frame_id);
  printf("Frame Size: %d\n", fhdr->frame_size);
  printf("Tag alter preservation: %s\n",
         fhdr->flags[0] & 64u >> 6 ? "Discard" : "Preserve");
  printf("File alter preservation: %s\n",
         fhdr->flags[0] & 32u >> 5 ? "Discard" : "Preserve");
  printf("Read Only: %s\n", fhdr->flags[0] & 16u >> 4 ? "True" : "False");
  printf("Group Frame: %s\n", fhdr->flags[1] & 64u >> 6 ? "True" : "False");
  printf("Compression: %s\n", fhdr->flags[1] & 8u >> 3 ? "True" : "False");
  printf("Encryption: %s\n", fhdr->flags[1] & 4u >> 2 ? "True" : "False");
  printf("Unsynchronization: %s\n",
         fhdr->flags[1] & 2u >> 1 ? "True" : "False");
  printf("Data length indicator: %s\n",
         fhdr->flags[1] & 1u >> 0 ? "Yes" : "No");
}

/*
 * Function stores id3 frame header into a struct and returns it
 * assumes fd is passed to it with its position at frame header
 */
struct frame_header *get_id3frameheader(int fd) {
  /* check if frame exists; return NULL if it doesn't */
  if (id3_framecheck(fd) == 0)
    return NULL;

  char frame_id[4];
  uint32_t size;
  uint8_t flags[2];
  struct frame_header *fhdr;

  /* allocate space for struct */
  fhdr = malloc(sizeof(struct frame_header));

  /* read 10 bytes from file */
  read(fd, frame_id, 4);
  read(fd, &size, 4);
  read(fd, flags, 2);

  /* change endianess of var `size` to big endian */
  size = __bswap_constant_32(size);

  /* print data */
  strcpy(fhdr->frame_id, frame_id);
  fhdr->frame_size = sync_safe_int_to_int(size);
  fhdr->flags[0] = flags[0] & 64u >> 6;
  fhdr->flags[1] = flags[0] & 32u >> 5;
  fhdr->flags[2] = flags[0] & 16u >> 4;
  fhdr->flags[3] = flags[1] & 64u >> 6;
  fhdr->flags[4] = flags[1] & 8u >> 3;
  fhdr->flags[5] = flags[1] & 4u >> 2;
  fhdr->flags[6] = flags[1] & 2u >> 1;
  fhdr->flags[7] = flags[1] & 1u >> 0;

  return fhdr;
}

/*
 * Function returns a pointer to an id3 frame
 */
struct frames *get_id3frame(int fd) {
  struct frames *frame;
  uint8_t *data;

  /* allocate space for frame */
  frame = malloc(sizeof(struct frames));

  /* if frame header is null then return null */
  frame->fhdr = get_id3frameheader(fd);
  if (frame->fhdr == NULL) {
    free(frame);
    return NULL;
  }

  /* allocate space for frame data */
  data = malloc(frame->fhdr->frame_size);
  frame->data = data;

  /* read data into frame data field */
  read(fd, frame->data, frame->fhdr->frame_size);

  return frame;
}
