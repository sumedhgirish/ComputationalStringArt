#pragma once
#include "images.h"

// Functions to manipulate images
NormImage* NormalizeRawImage(RawImage* inputImage);
NormImage* RgbToPrintable(RawImage* inputImage);
NormImage*
GetStringLengths(long int width, long int height, int resolution, double angles[], int nangles);
NormImage* RadonTransform(
	NormImage* inputimage, double angles[], int nangles, int nbins, int color, int resolution);
NormImage* NormRadonTransform(NormImage* inputimage,
							  double angles[],
							  int nangles,
							  int nbins,
							  int color,
							  int resolution,
							  NormImage* lenImage);
