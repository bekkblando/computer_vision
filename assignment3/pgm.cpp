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
    PGM *image = new PGM();

    // Get the pixels
    double* pixels;
    // A single whitespace - usually a newline
    fgetc(file);
    // Read in as doubles if under 256 1 double if over 2
    pixels = new double[row*column];

    // Note: Modeled after given assignment 1, the loading in I was using before loaded a block of data
    // which wouldn't work for this project
    unsigned char temp;
    for(int i=0; i < row; i++) {
        for(int j=0; j < column; j++) {
          if( (fread(&temp, sizeof(unsigned char), 1, file) < 1) || feof(file) )
            cout << "Error loading file\n";
          pixels[i*column + j] = (double)temp/(double)maxColor;
        }
      }

    image->name = file_name;
    image->magic = buff;
    image->cols = column;
    image->rows = row;
    image->maxc = maxColor;
    image->data = pixels;

    fclose(file);

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

    unsigned char* pixels = new unsigned char[(img->rows) * (img->cols)];
    for(int i = 0; i < img->cols * img->rows; i++)
        pixels[i] = (unsigned char)(img->data[i] * (double)255);

    // Write the data pixels, row, column, file
    fwrite(pixels, img->rows, img->cols, pgm);
    delete[] pixels;
    fclose(pgm);
}

void pgm_free(PGM* pgm){
    if(pgm->data)
        delete[] pgm->data;
}

double* createOpMem(double* data, int row, int col, int index){
    // I really didn't want to write out these checks. Had to make a few changes to
    // https://github.com/petermlm/SobelFilter/blob/master/src/sobel.c
    int top = index-col < 0;
    int bottom = index+col >= col*row;
    int left = index % col == 0;
    int right = (index+1) % col == 0;

    double* op_mem = new double[8];
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

double conv_x(double* data, int col, int row, int index, FLTR *filter){
    double* op_mem = createOpMem(data, row, col, index);
    float result = op_mem[0]*filter->filter[0] +  op_mem[3]*filter->filter[1] +  op_mem[6]*filter->filter[2] - 
            op_mem[2]*filter->filter[0] -  op_mem[5]*filter->filter[1] -  op_mem[8]*filter->filter[2];
    delete(op_mem);
    return(result);
}

double conv_y(double* data, int col, int row, int index, FLTR *filter){
    double* op_mem = createOpMem(data, row, col, index);
    double result = op_mem[0]*filter->filter[0] +  op_mem[1]*filter->filter[1] +  op_mem[2]*filter->filter[2] - 
            op_mem[6]*filter->filter[0] -  op_mem[7]*filter->filter[1] -  op_mem[8]*filter->filter[2];
    delete(op_mem);
    return(result);
}

PGM *pgm_conv(PGM *img, FLTR *filter){
    int row = img->rows, col = img->cols;
    double result;
    PGM *filtered = new PGM();
    filtered->name = nullptr;
    filtered->magic = new char[3];
    strcpy(filtered->magic,"P5");
    filtered->data = new double[row*col];
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
    double min = img->data[0], max = img->data[0];
    for(int i = 0; i < (img->rows * img->cols); i++){
        if(img->data[i] < min)
            min = img->data[i];
        if(img->data[i] > max)
            max = img->data[i];
    }

    for(int i = 0; i < (img->rows * img->cols); i++){
        img->data[i] = (((img->data[i] - min))/(max - min));
    }
}

PGM* contour_img(PGM *x, PGM *y){
    int row = x->rows, col = x->cols;
    PGM *contour = new PGM();
    contour->data = new double[row*col];
    contour->rows = row;
    contour->cols = col;
    contour->maxc = x->maxc;
    for(int i = 0; i < (row*col); i++){
        contour->data[i] = abs(x->data[i]) + abs(y->data[i]);
    }
    return(contour);
}

void dwt1d(double* data, int length, int levels){
    double s = sqrt(2.0);
    int k = pow(2, ((int)(floor(log2(length)))));
    double* temp = new double[length];
    for(int l = 0; l <= levels; l++){
        k = k/2;
        for(int i = 0; i < k; i++){
            temp[i] = (data[i*2] + data[i*2 + 1])/s;
            temp[i + k] = (data[i*2] - data[i*2 + 1])/s;
        }
        for(int i = 0; i < k * 2; i++){
            data[i] = temp[i];
        }
    }

}

void  pgm_dwt2D(PGM* img, int levels){
    // Apply 1d dwt to the columns
    for(int i = 0; i < img->cols; i++){
        dwt1d((img->data + (i * img->rows)), img->rows, levels);
    }

    // Apply 1d dwt to the rows
    for(int i = 0; i < img->rows; i++){
        dwt1d((img->data + (i * img->cols)), img->cols, levels);
    }
}
