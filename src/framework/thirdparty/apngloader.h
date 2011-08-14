#ifndef APNGLOADER_H
#define APNGLOADER_H

#include <sstream>

struct apng_data {
    unsigned char *pdata;
    unsigned int width;
    unsigned int height;
    unsigned int first_frame;
    unsigned int last_frame;
    unsigned char bpp;
    unsigned char coltype;
    unsigned int num_frames;
    unsigned int num_plays;
    unsigned int *frames_delay; // each frame delay in ms
};

// returns -1 on error, 0 on success
int load_apng(std::stringstream& file, struct apng_data *apng);
void free_apng(struct apng_data *apng);

#endif
