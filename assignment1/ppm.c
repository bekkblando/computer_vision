#include <stdio.h>
#include <stdlib.h> 
#include "ppm.h"


PPM *ppm_read(char *file_name){
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
    PPM *image = malloc(sizeof(PPM));

    // Get the pixels
    unsigned char* pixels;
    // A single whitespace - usually a newline
    fgetc(file);
    // Read in as unsigned chars if under 256 1 byte if over 2
    if(maxColor > 1){
        pixels = (unsigned char*)malloc(row*column*3*sizeof(unsigned char));
        fread(pixels, 3*row, column, file);
    }else{
        pixels = (unsigned char*)malloc(row*column*6*sizeof(unsigned char));
        fread(pixels, 6*row, column, file);
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

void ppm_write(PPM* img, const char *file){
    FILE *ppm = fopen( file, "w+");

    // Write the File Type
    fprintf(ppm, "P6\n");

    // Write the row and column
    fprintf(ppm, "%i %i\n", img->cols, img->rows);

    // Write the row and column
    fprintf(ppm, "%i\n", img->maxc);

    // Write the data pixels, 3*row, column, file
    fwrite(img->data, 3*(img->rows), img->cols, ppm);
    fclose(ppm);
}

PGM *ppm_to_pgm(PPM* ppm){
    int row = ppm->rows;
    int col = ppm->cols;
    PGM *image = malloc(sizeof(PGM));
    unsigned char* newPixels = (unsigned char*)malloc(sizeof(unsigned char)*row*col);
    unsigned char* pixels = ppm->data;

    // Covert the RGB pixels
    for(int pixel_index = 0; pixel_index < (row*col); pixel_index++){
        newPixels[pixel_index] = .21*pixels[(pixel_index*3)] + .72*pixels[(pixel_index*3)+1] + .07*pixels[(pixel_index*3)+2];
    }

    // Create and return the PGM
    image->name = ppm->name;
    image->cols = col;
    image->rows = row;
    image->maxc = ppm->maxc;
    image->magic = "P5";
    image->data = newPixels;

    return(image);
}

void ppm_free(PPM* ppm){
    free(ppm->data);
    free(ppm);
}