#include "filter.h"

FLTR  *make_filter(int filter_type){
    FLTR *filter = new FLTR();
    int x[] = {1, 2, 1};
    int y[] = {1, 0, -1};
    if(filter_type){
        filter->filter[0] = 1;
        filter->filter[1] = 2;
        filter->filter[2] = 1;
    }else{
        filter->filter[0] = 1;
        filter->filter[1] = 0;
        filter->filter[2] = -1;
    }
    filter->conv_type = filter_type;
    return(filter);
}