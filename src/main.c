#include "options.h"
#include "images.h"
#include "filters.h"

#include <stdio.h>

int main()
{
  #if DEBUG
    fprintf(stdout, "Started Program successfully!\n");
  #endif

  RawImage* imageData = LoadJpegFromPath("../images/nikhil.jpg"); /* Path is relative to directory program runs from */
  if (!imageData) {
    #if DEBUG
      fprintf(stdout, "Could not read image!\n");
    #endif
    return 1;
  }

  NormImage* printableData = RgbToPrintable(imageData);
  if (!printableData) {
    #if DEBUG
      fprintf(stdout, "Could not convert image to CMYK!\n");
    #endif
    return 1;
  }

  #if DEBUG
    fprintf(stdout, "Done!\n");
  #endif
  return 0;
}
