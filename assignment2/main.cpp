#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "filter.h"
#include "pgm.h"

int main(int argc,char **argv){
  PGM  *gimg;
  PGM  *dximg, *dyimg, *contour;
  FLTR  *gxp=make_filter(0), *gyp=make_filter(1);

  // allocte image of given size
  gimg = pgm_read(argv[1]);

  // convolve
  dximg = pgm_conv(gimg, gxp);
  dyimg = pgm_conv(gimg, gyp);

  // copy edge image and normalize, for visualization purposes

  contour = contour_img(dximg, dyimg);
  pgm_normalize(contour);
  pgm_write(contour, "contour.pgm");

  pgm_normalize(dximg);
  pgm_write(dximg, "normalized_dxgrey.pgm");
  pgm_normalize(dyimg);
  pgm_write(dyimg,"normalized_dygrey.pgm");



  // free image by sending to ppm_free function
  pgm_free(gimg);
  pgm_free(dximg);
  pgm_free(dyimg);

  return 0;
}
