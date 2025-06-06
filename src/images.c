#include "options.h"
#include "images.h"

#include <pngconf.h>
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <png.h>

RawImage* LoadJpegFromPath(const char* filename)
{
  // Allocate memory for libjpeg to report into in the current scope
  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr err;

  // create placeholder for loading image
  RawImage* newImage;

  // Initialize variables to hold data about the image
  unsigned long int imgWidth, imgHeight;
  int imgNumColorChannels;

  unsigned long int dwBufferBytes;

  unsigned char* imgRowBuffer[1];

  FILE* imgHandle;
  imgHandle = fopen(filename, "rb");
  if (imgHandle == NULL) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Failed to read image from file %s\n", __FILE__, __LINE__, filename);
    #endif
    // Return NULL if you can't read image into a pointer
    return NULL;
  }

  info.err = jpeg_std_error(&err);
  jpeg_create_decompress(&info);

  jpeg_stdio_src(&info, imgHandle);
  jpeg_read_header(&info, TRUE);
  jpeg_start_decompress(&info);

  // If decompression succeeds, only then will below code make sense
  imgWidth = info.image_width;
  imgHeight = info.image_height;
  imgNumColorChannels = info.num_components;

  #if DEBUG
    fprintf(stderr, "[INFO] <%s> Reading JPEG with size (%lu x %lu) and %u color channels\n",
            filename, imgWidth, imgHeight, imgNumColorChannels);
  #endif

  newImage = LoadRawImage(imgWidth, imgHeight, imgNumColorChannels);

  while (info.output_scanline < info.output_height) {
    // If 4 channels doesn't work, its because I changed 3 to info.num_components, try changing it back and above.
    imgRowBuffer[0] = (unsigned char *)(&newImage->data[info.num_components * info.output_width * info.output_scanline]);
    jpeg_read_scanlines(&info, imgRowBuffer, 1);
  }

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  fclose(imgHandle);

  return newImage;
}

RawImage* LoadPngFromPath(const char* filename)
{
  /* open file and test for it being a png */
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> File %s could not be opened for reading.\n", __FILE__, __LINE__, filename);
    #endif
    return NULL;
  }

  // 8 is the maximum size that can be checked
  char header[8];
  fread(header, 1, 8, fp);
  if (png_sig_cmp((png_const_bytep) header, 0, 8)){
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> File %s is not recognized as a PNG file.\n", __FILE__, __LINE__, filename);
    #endif
    return NULL;
  }


  png_structp pngptr;
  /* initialize stuff */
  pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngptr){
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> png_create_read_struct failed.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }

  png_infop infoptr;
  infoptr = png_create_info_struct(pngptr);
  if (!infoptr){
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> png_create_info_struct failed.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }

  if (setjmp(png_jmpbuf(pngptr))) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Error during init_io.\n", __FILE__, __LINE__);
    #endif
  }

  png_init_io(pngptr, fp);
  png_set_sig_bytes(pngptr, 8);

  png_read_info(pngptr, infoptr);

  RawImage* outputImage = LoadRawImage(png_get_image_width(pngptr, infoptr), png_get_image_height(pngptr, infoptr),
                                       png_get_channels(pngptr, infoptr));
  if (!outputImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Failed to create RawImage object.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  png_read_update_info(pngptr, infoptr);

  /* read file */
  if (setjmp(png_jmpbuf(pngptr))){
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Error during read_image.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }

  png_bytep rowptrs[outputImage->height];
  png_uint_32 rowbytes = png_get_rowbytes(pngptr, infoptr);
  for (int i = 0; i < outputImage->height; i++)
      rowptrs[i] = outputImage->data + i * rowbytes;

  png_read_image(pngptr, rowptrs);
  png_read_end(pngptr, NULL);
  fclose(fp);

  #if DEBUG
    fprintf(stderr, "[INFO] <%s> Reading PNG with size (%lu x %lu) and %u color channels\n",
            filename, outputImage->width, outputImage->height, outputImage->numColorChannels);
  #endif

  return outputImage;
}


RawImage* LoadRawImage(unsigned long width, unsigned long height, unsigned int numColorChannels)
{
  RawImage* outImage = malloc(sizeof(RawImage));
  if (!outImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Malloc failed for RawImage object!\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  outImage->width = width;
  outImage->height = height;
  outImage->numColorChannels = numColorChannels;

  outImage->data = (unsigned char *)malloc(sizeof(unsigned char) * width * height * numColorChannels);
  if (!outImage->data) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Could not allocate memory for RawImage databuffer!\n", __FILE__, __LINE__);
    #endif
    free(outImage);
    return NULL;
  }

  return outImage;
}

void UnloadRawImage(RawImage **image)
{
  #if DEBUG
    fprintf(stderr, "[INFO] <%s:%u> Called Free on raw-image.\n", __FILE__, __LINE__); 
  #endif
  free((*image)->data);
  free((*image));
  (*image)->data = NULL;
  *image = NULL;
}

NormImage* LoadNormImage(unsigned long width, unsigned long height, unsigned int numColorChannels)
{
  NormImage* outImage = malloc(sizeof(NormImage));
  if (!outImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Malloc failed for NormImage object!\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  outImage->width = width;
  outImage->height = height;
  outImage->numColorChannels = numColorChannels;

  outImage->data = (double *)calloc(width * height * numColorChannels, sizeof(double));
  if (!outImage->data) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Could not allocate memory for NormImage databuffer!\n", __FILE__, __LINE__);
    #endif
    free(outImage);
    return NULL;
  }

  return outImage;
}

void UnloadNormImage(NormImage **image)
{
  #if DEBUG
    fprintf(stderr, "[INFO] <%s:%u> Called Free on norm-image.\n", __FILE__, __LINE__); 
  #endif
  free((*image)->data);
  free(*image);
  (*image)->data = NULL;
  *image = NULL;
}
