#include <stdio.h>
#include "pgm.h"
#include "ppm.h"

int main(int argc, char *argv[]){
    PPM* ppm = ppm_read(argv[1]);
    PGM* pgm = ppm_to_pgm(ppm);
    pgm_write(pgm, argv[2]);
    ppm_free(ppm);
    pgm_free(pgm);
    return(0);
}