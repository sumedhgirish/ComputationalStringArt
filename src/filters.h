#pragma once
#include "images.h"

// Functions to manipulate images
NormImage* RgbToPrintable(RawImage* inputImage);
NormImage* RadonTransform(NormImage* inputimage, float angles[], int nangles, int nbins, int color, int resolution);
