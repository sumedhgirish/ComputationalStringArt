#pragma once
#include "images.h"

RawImage* ReduceRadonImage(NormImage *radonImage, double threshold, double line_thickness,
    double angles[], int nangles, double binSep, NormImage* lenImage);
