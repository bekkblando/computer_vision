#include <stdio.h>
#include <stdlib.h> 
#include <cmath>
#include "pgm.h"
#include "filter.h"
#include <iostream>
#include <algorithm>
#include <new> 
#include <iterator>
using namespace std;

PGM *pgm_read(char *file_name){
    FILE* file = fopen(file_name, "r");

    // Get File Type
    char buff[16];
    fgets(buff, 16, file);
    unsigned char c = fgetc(file); 
    // Get through comments
    while(c == '#'){
        while (fgetc(file) != '\n');
        c = fgetc(file);
    }
    
    // Get row, column
    ungetc(c, file);
    int row, column, maxColor;
    fscanf(file, "%i %i", &row, &column);

    fscanf(file, "%i", &maxColor);
    PGM *image = (PGM*)malloc(sizeof(PGM));

    // Get the pixels
    unsigned char* pixels;
    // A single whitespace - usually a newline
    fgetc(file);
    // Read in as unsigned chars if under 256 1 unsigned char if over 2
    if(maxColor < 256){
        pixels = (unsigned char*)malloc(row*column*sizeof(unsigned char));
        fread(pixels, row, column, file);
    }else{
        pixels = (unsigned char*)malloc(row*column*2*sizeof(unsigned char));
        fread(pixels, 2*row, column, file);
    }

    image->name = file_name;
    image->magic = buff;
    image->cols = column;
    image->rows = row;
    image->maxc = maxColor;
    image->magic = buff;
    image->data = pixels;

    return(image);
}


void pgm_write(PGM *img, const char *file){
    FILE *pgm = fopen( file, "w+");

    // Write the File Type
    fprintf(pgm, "P5\n");

    // Write the row and column
    fprintf(pgm, "%i %i\n", img->cols, img->rows);

    // Write the row and column
    fprintf(pgm, "%i\n", img->maxc);

    // for(int i = 0; i < img->rows; i++)
    //     printf("Test: %c\n", img->data[i]);

    // Write the data pixels, row, column, file
    fwrite(img->data, img->rows, img->cols, pgm);
    fclose(pgm);
}

void pgm_free(PGM* pgm){
    free(pgm->data);
    free(pgm);
}

unsigned char* createOpMem(unsigned char* data, int row, int col, int index){
    // I really didn't want to write out these checks. Had to make a few changes to
    // https://github.com/petermlm/SobelFilter/blob/master/src/sobel.c
    int top = index-col < 0;
    int bottom = index+col >= col*row;
    int left = index % col == 0;
    int right = (index+1) % col == 0;

    unsigned char* op_mem = new unsigned char[8];
    op_mem[0] = !top && !left     ? data[index+col-1] : 0;
    op_mem[1] = !top              ? data[index+col]   : 0;
    op_mem[2] = !top && !right    ? data[index+col+1] : 0;

    op_mem[3] = !left             ? data[index-1]       : 0;
    op_mem[4] = data[index];
    op_mem[5] = !right            ? data[index+1]       : 0;

    op_mem[6] = !bottom && !left  ? data[index-col-1] : 0;
    op_mem[7] = !bottom           ? data[index-col]   : 0;
    op_mem[8] = !bottom && !right ? data[index-col+1] : 0;
    return(op_mem);
}

unsigned char conv_x(unsigned char* data, int col, int row, int index, FLTR *filter){
    unsigned char* op_mem = createOpMem(data, row, col, index);
    float result = op_mem[0]*filter->filter[0] +  op_mem[3]*filter->filter[1] +  op_mem[6]*filter->filter[2] - 
            op_mem[2]*filter->filter[0] -  op_mem[5]*filter->filter[1] -  op_mem[8]*filter->filter[2];
    free(op_mem);
    return(result);
}

unsigned char conv_y(unsigned char* data, int col, int row, int index, FLTR *filter){
    unsigned char* op_mem = createOpMem(data, row, col, index);
    unsigned char result = op_mem[0]*filter->filter[0] +  op_mem[1]*filter->filter[1] +  op_mem[2]*filter->filter[2] - 
            op_mem[6]*filter->filter[0] -  op_mem[7]*filter->filter[1] -  op_mem[8]*filter->filter[2];
    free(op_mem);
    return(result);
}

PGM *pgm_conv(PGM *img, FLTR *filter){
    int row = img->rows, col = img->cols;
    unsigned char result;
    PGM *filtered = new PGM();
    filtered->data = (unsigned char*)malloc(row*col*sizeof(unsigned char));
    filtered->rows = row;
    filtered->cols = col;
    filtered->maxc = img->maxc;
    for(int i = 0; i < (row*col); i++){
        result = filter->conv_type ? conv_x(img->data, col, row, i, filter) : conv_y(img->data, col, row, i, filter);
        filtered->data[i] = result;
    }
    return(filtered);
}

void pgm_normalize(PGM *img){
    unsigned char min = img->data[0], max = img->data[0];
    for(int i = 0; i < (img->rows * img->cols); i++){
        if(img->data[i] < min)
            min = img->data[i];
        if(img->data[i] > max)
            max = img->data[i];
    }

    for(int i = 0; i < (img->rows * img->cols); i++){
        img->data[i] = (((img->data[i] - min)*(img->maxc))/(max - min));
    }
}

PGM* contour_img(PGM *x, PGM *y){
    int row = x->rows, col = x->cols;
    unsigned char result;
    PGM *contour = new PGM();
    contour->data = (unsigned char*)malloc(row*col*sizeof(unsigned char));
    contour->rows = row;
    contour->cols = col;
    contour->maxc = x->maxc;
    for(int i = 0; i < (row*col); i++){
        contour->data[i] = sqrt(pow(x->data[i], 2) + pow(y->data[i], 2));
    }
    return(contour);
}