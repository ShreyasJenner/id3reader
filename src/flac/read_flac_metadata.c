#include "flac/read_flac_metadata.h"
#include "stdheader.h"
#include <FLAC/metadata.h>
#include <stdio.h>

/* Function checks if the passed file is a flac file */
int flac_check(char *filename) {
  /* Declaration */
  int fd;
  char id[4];

  /* Open file */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening file");
    return -1;
  }

  /* set file descriptor to start of file */
  lseek(fd, 0, SEEK_SET);

  /* read first 4 identifying bytes */
  read(fd, id, 4);

  /* close fd */
  close(fd);

  return id[0] == 'f' && id[1] == 'L' && id[2] == 'a' && id[3] == 'C';
}

void read_flac(char *filename) {
  /* Declaration */
  int ch;
  bool flag;

  FLAC__Metadata_SimpleIterator *itr;
  FLAC__MetadataType metatype;
  FLAC__StreamMetadata *metadata;

  itr = FLAC__metadata_simple_iterator_new();
  if (itr == NULL) {
    fprintf(stderr, "Error creating flac metadata iterator\n");
    exit(1);
  }

  /* open the flac file */
  if (FLAC__metadata_simple_iterator_init(itr, filename, true, false) ==
      false) {
    fprintf(stderr, "Error creating flac metadata iterator\n");
    exit(1);
  }

  /* get block type */
  flag = true;
  ch = 'h';
  while (flag) {
    switch (ch) {
    case 'h':
      printf("Help:\n");
      printf("n: Move to next block\n");
      printf("p: Move to previous block\n");
      printf("d: Display current block; write image to output file if part of "
             "current block\n");
      printf("q: Quit\n");
      break;

    case 'p':
      if (FLAC__metadata_simple_iterator_prev(itr) == true) {
        printf("Moving to previous metadata block\n");
      } else {
        printf("At first block\n");
      }
      break;

    case 'n':
      if (FLAC__metadata_simple_iterator_next(itr) == true) {
        printf("Moving to next metadata block\n");
      } else {
        printf("At last block\n");
      }
      break;

    case 'd':
      metatype = FLAC__metadata_simple_iterator_get_block_type(itr);
      printf("Metatype: %d\n", metatype);

      metadata = FLAC__metadata_simple_iterator_get_block(itr);
      if (metadata == NULL) {
        fprintf(stderr, "Error getting metadata block\n");
        exit(1);
      }

      if (metatype == 0) {
        /* Stream info */
        printf("Stream Info\n");
        printf("Min Block size:%d\n", metadata->data.stream_info.min_blocksize);
        printf("Max Block size:%d\n", metadata->data.stream_info.max_blocksize);
        printf("Min Frame size:%d\n", metadata->data.stream_info.min_framesize);
        printf("Max Block size:%d\n", metadata->data.stream_info.max_framesize);
        printf("Sample rate:%d\n", metadata->data.stream_info.sample_rate);
        printf("Channels:%d\n", metadata->data.stream_info.channels);
        printf("Bits per sample:%d\n",
               metadata->data.stream_info.bits_per_sample);
        printf("Total Samples:%ld\n", metadata->data.stream_info.total_samples);
        printf("md5sum:%s\n", metadata->data.stream_info.md5sum);
      } else if (metatype == 1) {
        /* Padding */
        printf("Padding\n");
        printf("Dummy:%d\n", metadata->data.padding.dummy);
      } else if (metatype == 2) {
        /* Application */
        printf("Application\n");
        printf("Id: %s\n", metadata->data.application.id);
        printf("Data: %s\n", metadata->data.application.data);
      } else if (metatype == 3) {
        /* Seek Table */
        printf("Seek Table\n");
        printf("Num Points: %d\n", metadata->data.seek_table.num_points);
        printf("Seek Point sample number: %ld\n",
               metadata->data.seek_table.points->sample_number);
        printf("Seek Point stream offset: %ld\n",
               metadata->data.seek_table.points->stream_offset);
        printf("Seek Point frame samples: %d\n",
               metadata->data.seek_table.points->frame_samples);
      } else if (metatype == 4) {
        /* Vorbis Comment */
        printf("Vorbis Comment\n");
        printf("Vendor String Length:%d\n",
               metadata->data.vorbis_comment.vendor_string.length);
        printf("Vendor String Entry:%s\n",
               metadata->data.vorbis_comment.vendor_string.entry);
        printf("Num Comments: %d\n",
               metadata->data.vorbis_comment.num_comments);
        for (int i = 0; i < metadata->data.vorbis_comment.num_comments; i++) {
          printf("Vorbis Comment Length : %d\n",
                 metadata->data.vorbis_comment.comments[i].length);
          printf("Vorbis Comment Entry : %s\n",
                 metadata->data.vorbis_comment.comments[i].entry);
        }
      } else if (metatype == 5) {
        /* Cue Sheet */
        printf("Cue Sheet\n");
        printf("Media Catalog Number: %s\n",
               metadata->data.cue_sheet.media_catalog_number);
        printf("Lead in: %ld\n", metadata->data.cue_sheet.lead_in);
        printf("Is Cd: %b\n", metadata->data.cue_sheet.is_cd);
        printf("Num Tracks: %d\n", metadata->data.cue_sheet.num_tracks);

        printf("Cue Sheet Track\n");
        printf("Offset: %ld\n", metadata->data.cue_sheet.tracks->offset);
        printf("Number: %d\n", metadata->data.cue_sheet.tracks->number);
        printf("isrc: %s\n", metadata->data.cue_sheet.tracks->isrc);
        printf("type: %d\n", metadata->data.cue_sheet.tracks->type);
        printf("pre-emphasis: %d\n",
               metadata->data.cue_sheet.tracks->pre_emphasis);
        printf("Num Indices: %d\n",
               metadata->data.cue_sheet.tracks->num_indices);
        printf("Cue Sheet Index\n");
        printf("Offset: %ld\n",
               metadata->data.cue_sheet.tracks->indices->offset);
        printf("Number: %d\n",
               metadata->data.cue_sheet.tracks->indices->number);
      } else if (metatype == 6) {
        /* Picture */
        printf("Picture\n");
        printf("Mime type:%s\n", metadata->data.picture.mime_type);
        printf("Description: %s\n", metadata->data.picture.description);
        printf("Width: %d\n", metadata->data.picture.width);
        printf("Height: %d\n", metadata->data.picture.height);
        printf("Depth: %d\n", metadata->data.picture.depth);
        printf("Colors: %d\n", metadata->data.picture.colors);
        printf("Data Length: %d\n", metadata->data.picture.data_length);

        printf("Attempting to write image to .help/test.jpeg\n");
        int image_fd = open(".help/test.jpeg", O_WRONLY | O_CREAT, 0666);
        if (image_fd < 0) {
          fprintf(stderr, "Unable to open file\n");
          exit(1);
        }
        write(image_fd, metadata->data.picture.data,
              metadata->data.picture.data_length);
        close(image_fd);
        printf("Writing image to .help/test.jpeg\n");

        /* Unkown handling */
      } else if (metatype == 7) {
        printf("Unkown Block\n");
      }
      break;

    case 'q':
      printf("Quitting\n");
      flag = false;
      break;

    default:
      break;
    }

    /* get the input choice */
    if (flag && scanf("%d", &ch) != 1) {
      ch = fgetc(stdin);
    }
  }

  /* close the flac file */
  FLAC__metadata_simple_iterator_delete(itr);
}
