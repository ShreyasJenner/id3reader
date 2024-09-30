#include "images/jpeg.h"
#include "log.h"
#include "stdheader.h"

/* Function to get check if passed data contains jpeg data
 * Returns the starting position of the jpeg file if jpeg magic bytes are
 * identified
 * Returns -1 on failure
 * Magic bytes for jpeg file are ff d8 ff */
int check_JPEG(uint8_t *data, int buffer_size) {
  /* Declaration */
  int j;

  for (j = 2; j < buffer_size; j++) {
    if (data[j - 2] == 0xff && data[j - 1] == 0xd8 && data[j] == 0xff) {
      return j - 2;
    }
  }

  return -1;
}

/*
 * Function identifies the start of the jpeg data and writes it into a file
 * Magic bytes are: ff d8 ff
 * Return 0 on success
 * Returns non-zero number on failure
 */
int jpeg_writer(uint8_t *data, int buff_size) {
  /* Declaration */
  int fd, jpeg_pos;

  // open fd for writing image file
  fd = open(MP3_IMAGE, O_WRONLY | O_CREAT, 0666);
  if (fd < 0) {
    logerror(__FILE__, __LINE__, __func__, "Failed to create file descriptor");
    return -1;
  }

  /* get jpeg position and write the data into the file */
  jpeg_pos = check_JPEG(data, buff_size);
  if (jpeg_pos == -1) {
    printf("Not jpeg file\n");
    return -1;
  }
  write(fd, &data[jpeg_pos], buff_size - jpeg_pos);

  /* close the fd file and print output message */
  close(fd);
  printf("JPEG file written to %s\n", MP3_IMAGE);

  return 0;
}
