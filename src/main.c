#include "options.h"
#include "images.h"
#include "filters.h"

#include <stdio.h>
#include <math.h>

int main()
{
  #if DEBUG
    fprintf(stdout, "[INFO] Started Program successfully!\n");
  #endif

  /* HYPERPARAMS */
  const char* imageFile = "../images/naisha.jpeg";
  int totalNails = 2000;
  /* END HYPERPARAMS */

  RawImage* imageData = LoadJpegFromPath(imageFile); /* Path is relative to directory program runs from */
  if (!imageData) {
    #if DEBUG
      fprintf(stdout, "[WARNING] Could not read image!\n");
    #endif
    return 1;
  }

  // Compute frame with equal spacing with
  // totalNails nails in total around it
  totalNails += totalNails % 2;
  double origRatio = (float) imageData->width / imageData->height;
  int n = (int) floor( (origRatio / (origRatio + 1.)) * (totalNails / 2.) );
  double newRatio;
  if (fabs( (2. * n) / (totalNails - 2. * n) - origRatio ) < fabs(  (2. * n + 2.) / (totalNails - 2. * n - 2.) - origRatio  )) {
    newRatio = fabs( (2. * n) / (totalNails - 2. * n) );
  } else {
    newRatio = fabs(  (2. * n + 2.) / (totalNails - 2. * n - 2.) );
  }

  unsigned int baseNails = (int) n + 1;
  unsigned int heightNails = (int) (totalNails / 2) - n + 1;

  unsigned long int newWidth, newHeight;
  if (newRatio > origRatio) {
    newWidth = imageData->width;
    newHeight = (int) round( imageData->width / newRatio );
  } else {
    newHeight = imageData->height;
    newWidth = (int) round( imageData->height * newRatio );
  }
  double seperation = (double) newWidth / n;

  #if DEBUG
    fprintf(stdout, "[INFO] OrigRatio=%lf, NewRatio=%lf, NewDims=(%lu x %lu), baseNails=%u heightNails=%u, seperation=%lf\n",
            origRatio, newRatio, newWidth, newHeight, baseNails, heightNails, seperation);
  #endif

  if (newWidth > imageData->width || newHeight > imageData->height) {
    #if DEBUG
      fprintf(stderr, "[ERROR] <%s:%u> Invalid image ratio!\n", __FILE__, __LINE__);
    #endif
    return 1;
  }
  // End frame size calculation


  // Unload allocated memory
  UnloadRawImage(imageData);

  #if DEBUG
    fprintf(stdout, "[INFO] Done!\n");
  #endif
  return 0;
}
