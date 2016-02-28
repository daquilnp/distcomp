#include <stdio.h>
#include "a1.h"

// sliceNew can be smaller than slice by the amount of the oversent pixel data
inline int filter(int N, int width, int height, int AVERAGE, int QUICKSORT,
                  int startIndex, int endIndex, RGB *slice, RGB *sliceNew,
                  int end) {
  RGB *p         = sliceNew + (0);
  int  i         = 0;
  int  r         = 0, c = 0;
  int  np        = (N + 1) / 2;
  int  nm        = (N - 1) / 2;
  int  nSquared  = N * N;
  int  numValues = 0;
  int  cStart    = 0, cEnd = 0, rStart = 0, rEnd = 0;
  int  left      = 0, right = 0, up = 0, down = 0;
  int  colNeg    = 0, rowNeg = 0;
  int  col       = 0, row = 0;
  int  index     = 0;
  int  sumr      = 0, sumg = 0, sumb = 0;
  int  mIndex    = 0;
  int  wr        = 0;
  int  mLL       = 0;

  // printf("s[%d] e[%d] end[%d]\n", startIndex, endIndex, end); fflush(stdout);

  for (i = startIndex; i <= endIndex; i++) {
    /*
               image
        ^  |<--width->|
           |       ...
        h  |          |
        e  |==========|  <--- Oversent rows for Process N:  start of "slice"
        i  |----------|  <--- Process N starts: the value of (startIndex/width)
        g  |==========|
        h  |          |
        t  |==========|  <--- Oversent rows for Process N+1
        |  |----------|  <--- Process N end, Process N+1 starts
        |  |==========|  <--- Oversent rows for Process N
        ~  |          |
               ...
    */
    r = (startIndex / width) + floor((i - startIndex) / width);
    c = i % width;

    // default values that I change later
    cStart    = c - nm;
    cEnd      = c + nm;
    rStart    = r - nm;
    rEnd      = r + nm;
    numValues = nSquared;
    colNeg    = 0;
    rowNeg    = 0;

    // border check. l,r,u,d are true if
    // the current pixel has mask cells
    // that bleed over the edge
    left  = c < nm;
    right = c > (width - np);
    up    = r < nm;
    down  = end && (((endIndex - i) / width) < nm);

    if (left) {
      colNeg     = np - 1 - c;
      numValues -= (N * colNeg);
      cStart    += colNeg;
    } else if (right) {
      colNeg     = c - (width - np);
      numValues -= (N * colNeg);
      cEnd      -= colNeg;
    }

    if (up) {
      rowNeg     = np - 1 - r;
      numValues -= (N * rowNeg);
      rStart    += rowNeg;
    } else if (down) {
      rowNeg     = nm - (endIndex - i) / width;
      numValues -= (N * rowNeg);
      rEnd      -= rowNeg;
    }

    // correct the double-subtraction if a combo
    numValues += colNeg * rowNeg;

    sumr   = 0;
    sumg   = 0;
    sumb   = 0;
    mIndex = 0;
    wr     = 0;
    mLL    = (rEnd - rStart + 1) * (cEnd - cStart + 1);
    int medianListR[mLL]; // allocated on stack
    int medianListG[mLL]; // allocated on stack
    int medianListB[mLL]; // allocated on stack

    for (row = rStart; row <= rEnd; row++) {
      wr = width * row;

      for (col = cStart; col <= cEnd; col++) {
        index = wr + col;

        if (AVERAGE) {
          sumr += slice[index].r;
          sumg += slice[index].g;
          sumb += slice[index].b;
        } else {
          medianListR[mIndex] = slice[index].r;
          medianListG[mIndex] = slice[index].g;
          medianListB[mIndex] = slice[index].b;
          mIndex++;
        }
      }
    }

    p = sliceNew + (i - startIndex);

    if (AVERAGE) {
      p->r = sumr / nSquared;
      p->g = sumg / nSquared;
      p->b = sumb / nSquared;
    } else {
      if (QUICKSORT) {
        quick_sort(medianListR, 0, mLL);
        quick_sort(medianListG, 0, mLL);
        quick_sort(medianListB, 0, mLL);
      } else {
        bubble_sort(medianListR, mLL);
        bubble_sort(medianListG, mLL);
        bubble_sort(medianListB, mLL);
      }

      if (mLL % 2 == 0) {
        // even list, for the edges
        int m2 = mLL / 2;
        p->r = (medianListR[m2 - 1] + medianListR[m2]) / 2; // floor it
        p->g = (medianListG[m2 - 1] + medianListG[m2]) / 2; // floor it
        p->b = (medianListB[m2 - 1] + medianListB[m2]) / 2; // floor it
      } else {
        // odd list
        p->r = medianListR[nm];
        p->g = medianListG[nm];
        p->b = medianListB[nm];
      }
    }
  }
  return 0;
}

