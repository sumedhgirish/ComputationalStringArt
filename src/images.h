#pragma once

typedef struct RawImage RawImage;
typedef struct NormImage NormImage;

struct RawImage {
  unsigned int numColorChannels;
  unsigned long width, height;

  unsigned char* data;
};

struct NormImage {
  unsigned int numColorChannels;
  unsigned long width, height;

  double* data;
};

// Functions to load images of different file formats
RawImage* LoadJpegFromPath(const char* filename);
RawImage* LoadPngFromPath(const char* filename);
RawImage* LoadRawImage(unsigned long width, unsigned long height, unsigned int numColorChannels);
NormImage* LoadNormImage(unsigned long width, unsigned long height, unsigned int numColorChannels);


// Unloading counterparts
void UnloadRawImage(RawImage** image);
void UnloadNormImage(NormImage** image);
