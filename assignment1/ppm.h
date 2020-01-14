#ifndef PPM_H
#define PPM_H
#include "pgm.h"

typedef struct {
        char            *name;  // filename
        int             cols;   // width
        int             rows;   // height
        int             maxc;   // max color (usually 255)
        char            *magic; // magic number (expect 'P6')
        unsigned char           *data;  // the pixels, stored as 1D array
} PPM;

void     ppm_free(PPM *ppm);
PPM     *ppm_read(char *file_name);
void     ppm_write(PPM* img, const char *file);
PGM     *ppm_to_pgm(PPM *img);

#endif