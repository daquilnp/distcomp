#ifndef INCLUDED_A1_H
#define INCLUDED_A1_H

#include <math.h>

typedef struct {
  unsigned char r, g, b;
} RGB;

// Note: ONLY put integers in here. Code depends on it
typedef struct {
  int N;          // mask size is NxN
  int NP;         // number of processes, including master process
  int width;      // width of the original read in image, in pixels
  int height;     // height of the original read in image, in pixels
  int AVERAGE;    // boolean, if true do average, else do median
  int QUICKSORT;  // boolean, if true do quicksort for median,
                  // else do bubble sort
  int numPixels;  // the number of pixels that this process has to deal
                  // with, the size of my "slice"
  int startIndex; // a process is given numPixels pixels, but starts
                  // processing/filtering at startIndex
  int endIndex;   // a process is given numPixels pixels, but endIndex
                  // is the last one (inclusive) that needs to be filtered
                  // The difference of numPixels MINUS endIndex is the
                  // ovesent pixel data (just for the bottom) to help
                  // with mask calulation
} image_header_t;

RGB* readPPM(char *file,
             int  *width,
             int  *height,
             int  *max);
void writePPM(char      *file,
              int        width,
              int        height,
              int        max,
              const RGB *image);
void processImage(int  width,
                  int  height,
                  RGB *image);
int  filter(int  N,
            int  width,
            int  height,
            int  AVERAGE,
            int  QUICKSORT,
            int  startIndex,
            int  endIndex,
            RGB *slice,
            RGB *sliceNew,
            int  end);

#endif // ifndef INCLUDED_A1_H
