#include "images.h"
#include "options.h"
#include "stringart.h"

#include <stdio.h>
#include <float.h>

static double FindMax(double buff[], int n, unsigned long *index)
{
  double maxValue = -DBL_MAX;
  for (unsigned long i=0; i<n; ++i) {
    if (buff[i] > maxValue) {
      maxValue = buff[i];
      *index = i;
    }
  }
  return maxValue;
}

static void InplaceStringRadon(NormImage *imageBuffer, double alpha, double d, double lineLen, double lineThickness)
{
  if (!imageBuffer) {
    return;
  }


}

static void RemoveImageFrom(NormImage *targetImage, NormImage *dataImage)
{
  if (!(
    targetImage->width == dataImage->width &&
    targetImage->height == dataImage->height &&
    targetImage->numColorChannels == dataImage->numColorChannels
  )) {
    #if DEBUG
      fprintf(
        stderr,
        "[ERROR] <%s:%u> Cannot perform elementwise subtraction on images of differing sizes."
        " Got target = (%lu, %lu, %u), data = (%lu, %lu, %u)\n",
        __FILE__, __LINE__, targetImage->width, targetImage->height, targetImage->numColorChannels,
        dataImage->width, dataImage->height, dataImage->numColorChannels
      );
    #endif
    return;
  }

  for (int i=0; i<targetImage->width; ++i) {
    targetImage->data[i] -= dataImage->data[i];
  }
}

RawImage* ReduceRadonImage(NormImage *radonImage, double threshold, double lineThickness,
                           double angles[], int nangles, double binSep, NormImage *lenImage)
{
  RawImage* linesDrawn = LoadRawImage(radonImage->width, radonImage->height, 1);
  if (!linesDrawn) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Could not assign memory for strings drawn.\n", __FILE__, __LINE__);
    #endif
    return NULL;
  }

  NormImage* stringImage = LoadNormImage(radonImage->width, radonImage->height, 1);

  unsigned long buffLen = radonImage->width * radonImage->height, maxi = -1;
  double currMax;

  do {
    currMax = FindMax(radonImage->data, buffLen, &maxi);
    linesDrawn->data[maxi] = 1;
    InplaceStringRadon(stringImage, angles[maxi / radonImage->width],
                       maxi % radonImage->width * binSep, lenImage->data[maxi], lineThickness);
    RemoveImageFrom(radonImage, stringImage);
  } while (currMax > threshold);

  UnloadNormImage(&stringImage);
  return linesDrawn;
}

void GetStrings(NormImage* invImage)
{

}

