#include "options.h"
#include "images.h"
#include "filters.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define min(a, b) ((a < b) ? (a) : (b))

NormImage* RgbToPrintable(RawImage* inputImage)
{
  #if DEBUG
    fprintf(stderr, "[INFO] Converting image to printable colors!\n");
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


/*
* Radon Transform
*
* Split the pixels of the image into sub-pixels. For each angle, project the centers of the subpixels
* onto the line perpendicular to the line of given angle. Bin the subpixels based on
* ratio of distance to adjacent bins.
*
* If the given pixel is projected onto the bin exactly, give it the complete magnitude of
* the subpixel.
*
* MATLAB uses 4 subpixels per pixel
*
* resolution : how many subpixels make 1 side of a pixel (default 2)
*
*/

NormImage* RadonTransform(NormImage* inputImage, float angles[], int nangles, int nbins, int color, int resolution)
{
  NormImage* transformImage = malloc(sizeof(NormImage));
  if (!transformImage) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Failed to allocate memory for radon transform.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }
  transformImage->numColorChannels = 1;
  transformImage->width = nangles;
  transformImage->height = nbins;
  transformImage->data = (double *)calloc(nangles * nbins, sizeof(double));

  double pixVal, projVal, bini;
  int x, y, m, n;
  for (int anglei=0; anglei < nangles; ++anglei) {
    for (int pixi=0; pixi < inputImage->width * inputImage->height; ++pixi) {
      pixVal = inputImage->data[pixi * inputImage->numColorChannels + color];
      for (int subpixi=0; subpixi < resolution * resolution; ++subpixi) {
        x = pixi % inputImage->width;
        y = pixi / inputImage->width;
        m = subpixi % resolution;
        n = subpixi / resolution;

        projVal = modf(
          (x + ((double)(m + 1) / resolution) * cos(angles[anglei]) + (y + ((double)(n + 1) / resolution)) * sin(angles[anglei]),
          &bini);

        (transformImage->data + anglei * nbins)[(int)bini] += projVal * pixVal;
        (transformImage->data + anglei * nbins)[(int)bini] += (1 - projVal) * pixVal;
      }
    }
  }

  return transformImage;
}
