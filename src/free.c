#include <stdlib.h>
#include <stdbool.h>

#include "../include/free.h"
#include "../include/structs.h"

void ID3_FREE(struct id3_tag *tag) {
    struct frames *itr, *itr2;

    itr = tag->fms;
    while(itr!=NULL) {
        itr2 = itr->next_frame;
        free(itr->fhdr);
        free(itr);
        itr = itr2;
    }

    free(tag->hdr);
    free(tag);
}
