#include "jpeg_writer.h"
#include "stdheader.h"

/*
 * Function identifies the start of the jpeg data and writes it into a file
 * Magic bytes are: ff d8 ff e0
 * Return 0 on success
 * Returns non-zero number on failure
 */
int jpeg_writer(char *filename, int file_size, char *image_filename) {
  printf("Attempting to write jpeg file to %s\n", image_filename);

  // create a int8 array to store the image data
  uint8_t image[file_size], buff;
  int image_fd, bytes_read, bytes_written, fd;

  // open fd for writing image file
  fd = open(filename, O_RDONLY);
  image_fd = open(image_filename, O_WRONLY | O_CREAT, 0666);

  // check if file descriptor has been created
  if (image_fd < 0) {
    printf("Couldn't read file descriptor\n");
    return -1;
  }

  // loop to set fd to start of jpeg file
  while (1) {
    if (read(fd, &buff, 1) == 0)
      break;

    if (buff == 0xff) {
      read(fd, &buff, 1);
      if (buff == 0xd8) {
        read(fd, &buff, 1);
        if (buff == 0xff) {
          read(fd, &buff, 1);
          if (buff == 0xe0) {
            // move fd to start of magic bytes
            lseek(fd, -4, SEEK_CUR);
            printf("found jpeg\n");
            break;
          }
        }
      }
    }
  }

  // read the jpeg data into a buffer
  bytes_read = read(fd, image, file_size);
  // write the jpeg data into a buffer
  bytes_written = write(image_fd, image, file_size);

  // check if bytes read equals bytes written
  if (bytes_written != bytes_read) {
    printf("Bytes read != Bytes written\n");
    return -1;
  }
  printf("jpeg file written to %s\n", image_filename);

  return 0;
}
