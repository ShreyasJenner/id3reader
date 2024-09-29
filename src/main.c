#include "flac/read_flac_metadata.h"
#include "jpeg_writer.h"
#include "mp3/frame_reader.h"
#include "mp3/id3_free.h"
#include "mp3/id3_structs.h"
#include "mp3/id3reader.h"
#include "stdheader.h"
#include "syncint.h"

/* Driver Tag */
int main(int argc, char **argv) {
  /* Declaration */

  /* check if file arg passed */
  if (argc != 2) {
    fprintf(stderr, "Correct number of arguments not passed\n");
    exit(1);
  }

  /* check if passed file is mp3 */
  if (id3_tagcheck(argv[1])) {
    printf("mp3 file detected\n");
    id3_View(argv[1]);
  } else if (flac_check(argv[1])) {
    printf("Flac file identified\n");
    read_flac(argv[1]);
  } else {
    printf("Unkown file format\n");
  }

  return 0;
}
