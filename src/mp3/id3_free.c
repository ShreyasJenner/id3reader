#include "mp3/id3_free.h"
#include "mp3/id3_structs.h"
#include "stdheader.h"

void ID3_FREE(ID3Tag *tag) {
  int i;

  for (i = 0; i < tag->frame_no; i++) {
    free(tag->frame_arr[i]->hdr);
    free(tag->frame_arr[i]->data);
    free(tag->frame_arr[i]);
    free(tag->frame_list[i]);
  }

  free(tag->frame_list);
  free(tag->frame_arr);
  free(tag->hdr);
  free(tag);
}
