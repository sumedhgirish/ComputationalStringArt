#include "options.h"
#include "images.h"
#include "filters.h"

#include <stdlib.h>
#include <stdio.h>

#define min(a, b) ((a < b) ? (a) : (b))

NormImage* RgbToPrintable(RawImage* inputImage)
{
  #if DEBUG
    fprintf(stdout, "[INFO] Converting image to printable colors!\n");
  #endif

  if (inputImage->numColorChannels != 3) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Tried to convert non-rgb image into printable colors.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  NormImage* outputImage;
  unsigned long int monoChromeBufferWidth = inputImage->width * inputImage->height;

  outputImage = (NormImage *)malloc(sizeof(NormImage));
  if (!outputImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Failed to allocate memory for output image.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  // Reserve memory for C, M, Y, K, and W
  outputImage->data = (double *)malloc(sizeof(double) * monoChromeBufferWidth * 5);
  if (!outputImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Failed to allocate memory for output image buffer.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  outputImage->numColorChannels = 5;
  outputImage->width = inputImage->width;
  outputImage->height = inputImage->height;

  // Translate rgb into cmy
  for (int color=0; color<3; ++color) {
    for (int i=0; i<monoChromeBufferWidth; ++i) {
      outputImage->data[i*5 + color] = ((float) 255 - inputImage->data[i*3 + color]) / 255;
    }
  }

  // Prepare k stream
  for (int i=0; i<monoChromeBufferWidth; ++i) {
    outputImage->data[i*5 + 3] = min(min(min(outputImage->data[i*5], outputImage->data[i*5 + 1]), outputImage->data[i*5 + 2]), 0.999);
  }

  // remove redundant black from cmy
  for (int color=0; color<3; ++color) {
    for (int i=0; i<monoChromeBufferWidth; ++i) {
      outputImage->data[i*5 + color] -= outputImage->data[i*5 + 3];
      outputImage->data[i*5 + color] /= (1 - outputImage->data[i*5 + 3]);
    }
  }

  // Prepare w stream
  for (int i=0; i<monoChromeBufferWidth; ++i) {
    outputImage->data[i*5 + 4] = (
      1 - (outputImage->data[i*5] + outputImage->data[i*5 + 1] + outputImage->data[i*5 + 2])/3
    );
  }

  return outputImage;
}
