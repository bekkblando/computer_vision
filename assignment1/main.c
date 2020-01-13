#include <stdio.h>
#include "pgm.h"
#include "ppm.h"

int main(){
    PPM* ppm = ppm_read("./mandrill.ppm");
    PGM* pgm = ppm_to_pgm(ppm);
    pgm_write(pgm, "test.ppm");
    ppm_free(ppm);
    pgm_free(pgm);
    return(0);
}