#ifndef PGM_H
#define PGM_H

typedef struct {
        char            *name;  // filename
        int             cols;   // width
        int             rows;   // height
        int             maxc;   // max color (usually 255)
        char            *magic; // magic number (expect 'P5')
        unsigned char           *data;  // the pixels, stored as 1D array
} PGM;

void     pgm_free(PGM *pgm);
PGM     *pgm_read(const char *file_name);
void      pgm_write(PGM *img, const char *file);
void     pgm_white(PGM *img);

#endif