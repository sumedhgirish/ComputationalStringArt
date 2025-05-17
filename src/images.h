#pragma once


typedef struct RawImage RawImage;

struct RawImage {
  unsigned int numColorChannels;
  unsigned long width, height;

  unsigned char* data;
};


RawImage* LoadJpegFromPath(const char* filename);
