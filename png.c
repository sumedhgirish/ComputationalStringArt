#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <png.h>

typedef struct {
    int rows;
    int cols;
    int **rmap;
    int **gmap;
    int **bmap;
} Image;

Image* readPNGImage(char* filename);
void writePPMImage(Image *image, char *filename);

int main(int argc, char **argv) {
    Image* image = readPNGImage("image.png");
    writePPMImage(image, "out.ppm");

    return 0;
}

Image* readPNGImage(char* filename){
    Image *image = malloc(sizeof(Image));
    png_byte color_type;
    png_bytep * row_pointers;
    png_structp png_ptr;
    png_infop info_ptr;
    int i, j;

    // 8 is the maximum size that can be checked
    char header[8];

    /* open file and test for it being a png */
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "[read_png_file] File %s could not be opened for reading", filename);
        return NULL;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)){
       fprintf(stderr, "[read_png_file] File %s is not recognized as a PNG file", filename);
       return NULL;
    }


    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr){
        fprintf(stderr, "[read_png_file] png_create_read_struct failed");
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        fprintf(stderr, "[read_png_file] png_create_info_struct failed");
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        fprintf(stderr, "[read_png_file] Error during init_io");
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    image->cols = png_get_image_width(png_ptr, info_ptr);
    image->rows = png_get_image_height(png_ptr, info_ptr);

    color_type = png_get_color_type(png_ptr, info_ptr);
    if(color_type != PNG_COLOR_TYPE_RGB){
        fprintf(stderr, "[read_png_file] Only RGB PNGs are supported");
        return NULL;
    }
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))){
        fprintf(stderr, "[read_png_file] Error during read_image");
        return NULL;
    }

    /* memory allocation */
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image->rows);
    for (i = 0; i < image->rows; i += 1){
        row_pointers[i] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    image->rmap = malloc(image->rows * sizeof(int *));
    for (i = 0; i < image->rows; i += 1)
        (image->rmap)[i] =  malloc(image->cols * sizeof(int));

    image->gmap = malloc(image->rows * sizeof(int *));
    for (i = 0; i < image->rows; i += 1)
        (image->gmap)[i] = malloc(image->cols * sizeof(int));

    image->bmap = malloc(image->rows * sizeof(int *));
    for (i = 0; i < image->rows; i += 1)
        (image->bmap)[i] = malloc(image->cols * sizeof(int));

    png_read_image(png_ptr, row_pointers);
    fclose(fp);

    for (i = 0; i < image->rows; i += 1) {
        png_byte* row = row_pointers[i];
        for (j = 0; j < image->cols; j += 1) {
            png_byte* ptr = &(row[j * 3]);

            image->rmap[i][j] = ptr[0];
            image->gmap[i][j] = ptr[1];
            image->bmap[i][j] = ptr[2];
        }
    }

    /* clean up */
    for (i = 0; i < image->rows; i += 1){
        free(row_pointers[i]);
    }
    free(row_pointers);

    return image;
}

void writePPMImage(Image *image, char *filename){
    FILE* fp;
    int i, j;

    /* Opening */
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("error in opening file %s\n", filename);
        exit(1);
    }

    /*  Magic number writing */
    fprintf(fp, "P6\n");

    /* Dimensions */
    fprintf(fp, "%d %d \n", image->cols, image->rows);

    /* Max val */
    fprintf(fp, "%d\n", 255);

    /* Pixel values */
    for(i = 0; i < image->rows; i += 1){
        for(j = 0; j < image->cols ; j += 1){
            fprintf(fp, "%c", image->rmap[i][j]);
            fprintf(fp, "%c", image->gmap[i][j]);
            fprintf(fp, "%c", image->bmap[i][j]);
        }
    }

    /* Close file */
    fclose(fp);
}
