#include "options.h"
#include "images.h"

#include <stdio.h>

int main()
{
  #if DEBUG
    fprintf(stdout, "Started Program successfully!\n");
  #endif

  RawImage* naisha = LoadJpegFromPath("../images/naisha.jpeg");

  #if DEBUG
    fprintf(stdout, "Done!\n");
  #endif
  return 0;
}
