#include "options.h"
#include "images.h"

#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <stdlib.h>

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
  unsigned char* imgData;

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
    fprintf(stdout, "[INFO] <%s> Reading JPEG with size (%lu x %lu) and %u color channels\n",
            filename, imgWidth, imgHeight, imgNumColorChannels);
  #endif

  dwBufferBytes = imgWidth * imgHeight * 3; /* Only read in RGB and not A */
  imgData = (unsigned char*)malloc(sizeof(unsigned char) * dwBufferBytes);

  newImage = (RawImage*)malloc(sizeof(RawImage));
  newImage->width = imgWidth;
  newImage->height = imgHeight;
  newImage->data = imgData;

  while (info.output_scanline < info.output_height) {
    imgRowBuffer[0] = (unsigned char *)(&imgData[3 * info.output_width * info.output_scanline]);
    jpeg_read_scanlines(&info, imgRowBuffer, 1);
  }

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  fclose(imgHandle);

  return newImage;
}
