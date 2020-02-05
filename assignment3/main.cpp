#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "pgm.h"

int main(int argc, char **argv){
  PGM  *gimg;

  // allocte image of given size
  gimg = pgm_read("mandrill.pgm");

  pgm_dwt2D(gimg, 1);
  pgm_normalize(gimg);
  pgm_write(gimg, "test.pgm");


  // free image by sending to ppm_free function
  pgm_free(gimg);

  return 0;
}
