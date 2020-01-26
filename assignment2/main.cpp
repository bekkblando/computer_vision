#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "filter.h"
#include "pgm.h"

int main(int argc,char **argv){
  std::cout << "We are here";
  PGM  *gimg;
  PGM  *ndximg, *ndyimg;
  PGM  *dximg, *dyimg;
  std::cout << "We are here";
  FLTR  *gxp=make_filter(0), *gyp=make_filter(1);
  std::cout << "We are here";

  // allocte image of given size
  gimg = pgm_read("mandrill.pgm");
//   pgm_write(gimg,"test.pgm");

//   // convolve
  dximg = pgm_conv(gimg,gxp);
  dyimg = pgm_conv(gimg,gyp);

//   // copy edge image and normalize, for visualization purposes
// //   ndximg = pgm_copy(dximg);
// //   pgm_normalize(ndximg);
  pgm_write(dximg, "ndxgrey.pgm");
//   pgm_write(dyimg,"ndygrey.pgm");
//   ndyimg = pgm_copy(dyimg);
//   pgm_normalize(ndyimg);
//   pgm_write(ndyimg,"ndygrey.pgm");

//   // output to file
//   pgm_write(dximg,"dxgrey.pgm");
//   pgm_write(dyimg,"dygrey.pgm");

//   // free image by sending pointer to pointer to ppm_free function
//   pgm_free(&dximg);
//   pgm_free(&dyimg);
//   pgm_free(&gimg);
//   free_filter(&gxp);
//   free_filter(&gyp);

  return 0;
}
