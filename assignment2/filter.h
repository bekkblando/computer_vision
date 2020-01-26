#ifndef FLTR_H
#define FLTR_H

typedef struct {
        int  filter[3];  // The Convolution
        int conv_type;
} FLTR;

FLTR     *make_filter(int filter_type);


#endif