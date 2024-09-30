#include "flac/read_flac_metadata.h"
#include "images/jpeg.h"
#include "log.h"
#include "mp3/frame_reader.h"
#include "mp3/id3_free.h"
#include "mp3/id3_structs.h"
#include "mp3/id3reader.h"
#include "stdheader.h"
#include "syncint.h"

/* Driver Tag */
int main(int argc, char **argv) {
  /* check if file arg passed */
  if (argc < 2) {
    logerror(__FILE__, __LINE__, __func__, "Argument missing");
    exit(1);
  }

  /* check if passed file is mp3 */
  if (id3_tagcheck(argv[1])) {
    printf("mp3 file detected\n");
    id3_View(argv[1]);
  } else if (flac_check(argv[1])) {
    printf("Flac file identified\n");
    view_FLACMetadata(argv[1]);
  } else {
    printf("Unkown file format\n");
  }

  return 0;
}
