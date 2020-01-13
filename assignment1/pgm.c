#include <stdio.h>
#include <stdlib.h> 
#include "pgm.h"

void pgm_write(PGM *img, const char *file){
    FILE *pgm = fopen( file, "w+");

    // Write the File Type
    fprintf(pgm, "P5\n");

    // Write the row and column
    fprintf(pgm, "%i %i\n", img->cols, img->rows);

    // Write the row and column
    fprintf(pgm, "%i\n", img->maxc);

    // Write the data pixels, row, column, file
    fwrite(img->data, img->rows, img->cols, pgm);
    fclose(pgm);
}

void pgm_free(PGM* pgm){
    free(pgm->data);
    free(pgm);
}