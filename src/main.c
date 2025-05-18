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

  // write data to file
  FILE* outFile;
  outFile = fopen("../output/imagepixels.txt", "w");
  if (!outFile) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Could not open text file for writing.\n", __FILE__, __LINE__);
    #endif
    return 1;
  }
  for (int i=0; i < printableData->width * printableData->height * 5; ++i) {
    fprintf(outFile, "%lf\n", printableData->data[i]);
  }
  fclose(outFile);

  UnloadRawImage(imageData);
  UnloadNormImage(printableData);

  #if DEBUG
    fprintf(stdout, "Done!\n");
  #endif
  return 0;
}
