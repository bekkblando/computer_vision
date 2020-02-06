#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "pgm.h"

int main(int argc, char **argv){
  PGM  *gimg;
  int levels = 1;

  // allocte image of given size
  gimg = pgm_read("mandrill.pgm");

  pgm_disc_wave_2d(gimg, levels);
  pgm_normalize(gimg);
  pgm_write(gimg, "test.pgm");

  pgm_inverse_disc_wave_2d(gimg, levels);
  pgm_normalize(gimg);
  pgm_write(gimg, "inverse_test.pgm");





  // free image by sending to ppm_free function
  // pgm_free(gimg);

  return 0;
}
