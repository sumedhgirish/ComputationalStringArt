#pragma once
#include "images.h"

RawImage* ReduceRadonImage(NormImage *radonImage, double threshold, double line_thickness,
    unsigned int max_nails, double angles[], int nangles);
