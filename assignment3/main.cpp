#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "pgm.h"

int main(int argc, char **argv){
  PGM  *gimg, *g_copy;
  int levels = 1;

  // allocte image of given size
  gimg = pgm_read(argv[1]);
  levels = std::stoi(argv[2]);

  pgm_disc_wave_2d(gimg, levels);
  g_copy = pgm_copy(gimg);
  pgm_normalize(gimg);
  pgm_write(gimg, "normalized.pgm");

  pgm_inverse_disc_wave_2d(g_copy, levels);
  pgm_normalize(g_copy);
  pgm_write(g_copy, "inverse_test.pgm");





  // free image by sending to ppm_free function
  pgm_free(g_copy);
  pgm_free(gimg);

  return 0;
}
