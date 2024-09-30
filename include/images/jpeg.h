#ifndef JPEG_WRITER_H
#define JPEG_WRITER_H

#include "stdheader.h"

int check_JPEG(uint8_t *data, int buffer_size);

int jpeg_writer(uint8_t *data, int buff_size);

#endif
