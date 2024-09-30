#include "flac/flac_structs.h"
#include "flac/read_flac_metadata.h"
#include "stdheader.h"

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

FLACMetadata *allocate_FLACMetadataSpace() {
  /* Declaration */
  FLACMetadata *metadata;

  /* Allocate space for the metadata struct */
  metadata = malloc(sizeof(FLACMetadata));
  if (metadata == NULL) {
    fprintf(stderr, "Error creating metadata struct");
    return NULL;
  }

  // metadata->streaminfo = malloc(sizeof(FLAC__StreamMetadata_StreamInfo));
  // metadata->padding = malloc(sizeof(FLAC__StreamMetadata_Padding));
  // metadata->app = malloc(sizeof(FLAC__StreamMetadata_Application));
  // metadata->seektable = malloc(sizeof(FLAC__StreamMetadata_SeekTable));
  // metadata->vorbis_comment =
  // malloc(sizeof(FLAC__StreamMetadata_VorbisComment)); metadata->cue_sheet =
  // malloc(sizeof(FLAC__StreamMetadata_CueSheet)); metadata->picture =
  // malloc(sizeof(FLAC__StreamMetadata_Picture)); if (metadata->streaminfo ==
  // NULL || metadata->padding == NULL ||
  //     metadata->app == NULL || metadata->seektable == NULL ||
  //     metadata->vorbis_comment == NULL || metadata->cue_sheet == NULL ||
  //     metadata->picture == NULL) {
  //   fprintf(stderr, "Error allocating space for FLACMetadata memebers\n");
  //   return NULL;
  // }

  return metadata;
}

void store_FLACMetadata(FLACMetadata *metadata, FLAC__MetadataType metatype,
                        FLAC__StreamMetadata *streammetadata) {

  /* store data based on metatype */
  if (metatype == 0) {
    /* Stream info */
    metadata->streaminfo = streammetadata;
    printf("stream meta info stored\n");
  } else if (metatype == 1) { /* Padding */
    metadata->padding = streammetadata;
  } else if (metatype == 2) {
    /* Application */
    metadata->app = streammetadata;
  } else if (metatype == 3) {
    /* Seek Table */
    metadata->seektable = streammetadata;
  } else if (metatype == 4) {
    /* Vorbis Comment */
    metadata->vorbis_comment = streammetadata;
  } else if (metatype == 5) {
    /* Cue Sheet */
    metadata->cue_sheet = streammetadata;
  } else if (metatype == 6) {
    /* Picture */
    metadata->picture = streammetadata;
    /* Unkown handling */
  } else if (metatype == 7) {
    printf("Unkown Block\n");
  }
}

FLACMetadata *get_FLACMetadata(char *filename) {
  /* Declaration */
  FLACMetadata *metadata;

  FLAC__Metadata_SimpleIterator *itr;
  FLAC__MetadataType metatype;
  FLAC__StreamMetadata *streammetadata;

  itr = FLAC__metadata_simple_iterator_new();
  if (itr == NULL) {
    fprintf(stderr, "Error creating flac metadata iterator\n");
    return NULL;
  }

  /* allocate space ofr flac metadata */
  metadata = allocate_FLACMetadataSpace();
  if (metadata == NULL) {
    fprintf(stderr, "Error -> allocate_FLACMetadataSpace");
    return NULL;
  }

  /* open the flac file */
  if (FLAC__metadata_simple_iterator_init(itr, filename, true, false) ==
      false) {
    fprintf(stderr, "Error creating flac metadata iterator\n");
    return NULL;
  }

  /* Store the flac data into the FLACMetadata struct */
  do {
    /* get the metatype and stream metadata and allocate it to correct memeber
     */
    metatype = FLAC__metadata_simple_iterator_get_block_type(itr);
    streammetadata = FLAC__metadata_simple_iterator_get_block(itr);
    store_FLACMetadata(metadata, metatype, streammetadata);
  } while (FLAC__metadata_simple_iterator_next(itr));

  /* close iterator */
  FLAC__metadata_simple_iterator_delete(itr);

  return metadata;
}

void clean_FLACMetadata(FLACMetadata *metadata) {
  if (metadata->picture != NULL) {
    FLAC__metadata_object_delete(metadata->picture);
  }
  if (metadata->cue_sheet != NULL) {
    FLAC__metadata_object_delete(metadata->cue_sheet);
  }
  if (metadata->vorbis_comment != NULL) {
    FLAC__metadata_object_delete(metadata->vorbis_comment);
  }
  if (metadata->seektable != NULL) {
    FLAC__metadata_object_delete(metadata->seektable);
  }
  if (metadata->app != NULL) {
    FLAC__metadata_object_delete(metadata->app);
  }
  if (metadata->padding != NULL) {
    FLAC__metadata_object_delete(metadata->padding);
  }
  if (metadata->streaminfo != NULL) {
    FLAC__metadata_object_delete(metadata->streaminfo);
  }

  free(metadata);
}

/* Function to view flac metadata in file
 * Handles creation and destruction of metadata */
void view_FLACMetadata(char *filename) {
  /* Declaration */
  int ch, flag;
  FLACMetadata *metadata;

  /* Initialization */
  flag = 1;
  ch = 'h';

  /* get the flac metadata */
  metadata = get_FLACMetadata(filename);
  if (metadata == NULL) {
    fprintf(stderr, "Error getting flac metadata\n");
    return;
  }

  /* view flac metadata */
  while (flag) {
    switch (ch) {
    case 'h':
      printf("Help:\n");
      printf("s: Print Stream info\n");
      printf("p: Print Padding\n");
      printf("a: Print Application\n");
      printf("t: Print Seek Table\n");
      printf("v: Print Vorbis Comment\n");
      printf("c: Print Cue Sheet\n");
      printf("i: Write image to a file\n");
      printf("q: Quit\n");
      break;

    case 's':
      /* Stream info */
      if (metadata->streaminfo != NULL) {
        printf("Stream Info\n");
        printf("Min Block size:%d\n",
               metadata->streaminfo->data.stream_info.min_blocksize);
        printf("Max Block size:%d\n",
               metadata->streaminfo->data.stream_info.max_blocksize);
        printf("Min Frame size:%d\n",
               metadata->streaminfo->data.stream_info.min_framesize);
        printf("Max Block size:%d\n",
               metadata->streaminfo->data.stream_info.max_framesize);
        printf("Sample rate:%d\n",
               metadata->streaminfo->data.stream_info.sample_rate);
        printf("Channels:%d\n",
               metadata->streaminfo->data.stream_info.channels);
        printf("Bits per sample:%d\n",
               metadata->streaminfo->data.stream_info.bits_per_sample);
        printf("Total Samples:%ld\n",
               metadata->streaminfo->data.stream_info.total_samples);
        printf("md5sum:%s\n", metadata->streaminfo->data.stream_info.md5sum);
      } else {
        printf("Stream Info not present\n");
      }
      break;

    case 'p':
      /* Padding */
      if (metadata->padding != NULL) {
        printf("Padding\n");
        printf("Dummy:%d\n", metadata->padding->data.padding.dummy);
      } else {
        printf("Padding not present\n");
      }
      break;

    case 'a':
      /* Application */
      if (metadata->app != NULL) {
        printf("Application\n");
        printf("Id: %s\n", metadata->app->data.application.id);
        printf("Data: %s\n", metadata->app->data.application.data);
      } else {
        printf("Application not present\n");
      }
      break;

    case 't':
      /* Seek Table */
      if (metadata->seektable != NULL) {
        printf("Seek Table\n");
        printf("Num Points: %d\n",
               metadata->seektable->data.seek_table.num_points);
        printf("Seek Point sample number: %ld\n",
               metadata->seektable->data.seek_table.points->sample_number);
        printf("Seek Point stream offset: %ld\n",
               metadata->seektable->data.seek_table.points->stream_offset);
        printf("Seek Point frame samples: %d\n",
               metadata->seektable->data.seek_table.points->frame_samples);
      } else {
        printf("Seek Table not present\n");
      }
      break;

    case 'v':
      /* Vorbis Comment */
      if (metadata->vorbis_comment != NULL) {
        printf("Vorbis Comment\n");
        printf(
            "Vendor String Length:%d\n",
            metadata->vorbis_comment->data.vorbis_comment.vendor_string.length);
        printf(
            "Vendor String Entry:%s\n",
            metadata->vorbis_comment->data.vorbis_comment.vendor_string.entry);
        printf("Num Comments: %d\n",
               metadata->vorbis_comment->data.vorbis_comment.num_comments);
        for (int i = 0;
             i < metadata->vorbis_comment->data.vorbis_comment.num_comments;
             i++) {
          printf(
              "Vorbis Comment Length : %d\n",
              metadata->vorbis_comment->data.vorbis_comment.comments[i].length);
          printf(
              "Vorbis Comment Entry : %s\n",
              metadata->vorbis_comment->data.vorbis_comment.comments[i].entry);
        }
      } else {
        printf("Vorbis Comment not present\n");
      }

      break;

    case 'c':
      /* Cue Sheet */
      if (metadata->cue_sheet != NULL) {
        printf("Cue Sheet\n");
        printf("Media Catalog Number: %s\n",
               metadata->cue_sheet->data.cue_sheet.media_catalog_number);
        printf("Lead in: %ld\n", metadata->cue_sheet->data.cue_sheet.lead_in);
        printf("Is Cd: %b\n", metadata->cue_sheet->data.cue_sheet.is_cd);
        printf("Num Tracks: %d\n",
               metadata->cue_sheet->data.cue_sheet.num_tracks);

        printf("Cue Sheet Track\n");
        printf("Offset: %ld\n",
               metadata->cue_sheet->data.cue_sheet.tracks->offset);
        printf("Number: %d\n",
               metadata->cue_sheet->data.cue_sheet.tracks->number);
        printf("isrc: %s\n", metadata->cue_sheet->data.cue_sheet.tracks->isrc);
        printf("type: %d\n", metadata->cue_sheet->data.cue_sheet.tracks->type);
        printf("pre-emphasis: %d\n",
               metadata->cue_sheet->data.cue_sheet.tracks->pre_emphasis);
        printf("Num Indices: %d\n",
               metadata->cue_sheet->data.cue_sheet.tracks->num_indices);
        printf("Cue Sheet Index\n");
        printf("Offset: %ld\n",
               metadata->cue_sheet->data.cue_sheet.tracks->indices->offset);
        printf("Number: %d\n",
               metadata->cue_sheet->data.cue_sheet.tracks->indices->number);
      } else {
        printf("Cue Sheet not present\n");
      }
      break;

    case 'i':
      /* Picture */
      if (metadata->picture != NULL) {
        printf("Picture\n");
        printf("Mime type:%s\n", metadata->picture->data.picture.mime_type);
        printf("Description: %s\n",
               metadata->picture->data.picture.description);
        printf("Width: %d\n", metadata->picture->data.picture.width);
        printf("Height: %d\n", metadata->picture->data.picture.height);
        printf("Depth: %d\n", metadata->picture->data.picture.depth);
        printf("Colors: %d\n", metadata->picture->data.picture.colors);
        printf("Data Length: %d\n",
               metadata->picture->data.picture.data_length);

        printf("Attempting to write image to %s\n", FLAC_IMAGE);
        int image_fd = open(FLAC_IMAGE, O_WRONLY | O_CREAT, 0666);
        if (image_fd < 0) {
          fprintf(stderr, "Unable to open file\n");
          clean_FLACMetadata(metadata);
          return;
        }
        write(image_fd, metadata->picture->data.picture.data,
              metadata->picture->data.picture.data_length);
        close(image_fd);
        printf("Writing image to %s\n", FLAC_IMAGE);
      } else {
        printf("Picture not present\n");
      }

      break;

    case 'q':
      printf("Quitting\n");
      flag = 0;
      break;

    default:
      break;
    }

    /* get the input choice */
    if (flag && scanf("%d", &ch) != 1) {
      ch = fgetc(stdin);
    }
  }

  /* delete the block */
  clean_FLACMetadata(metadata);
}
