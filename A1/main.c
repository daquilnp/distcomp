/* ass1.c modifed from mess.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mpi.h"
#include "a1.h"

#define BUF_SIZE           (1000)
#define EXPECTED_ARG_COUNT (1 + 4) // name of program + 4 args
#define TRUE               (1)
#define FALSE              (0)
#define MASTER_PROCESS     (0)

#define SEND_AMT           ih.endIndex - ih.startIndex + 1

int main(int argc, char *argv[])
{
  if (argc != EXPECTED_ARG_COUNT) {
    printf("Expected [%d] arguments, got [%d]...exiting \n",
           EXPECTED_ARG_COUNT, argc);
    exit(-1);
  }

  int   pin, NP;          // process identification number, number of processes
  int   sendPin, recvPin; // pin of sender and receiver
  char *sendbuf, *recvbuf;
  int   tag = 0;
  image_header_t ih;
  MPI_Status     status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &NP);
  MPI_Comm_rank(MPI_COMM_WORLD, &pin);

  if (pin == 0) {
    // this is the MASTER_PROCESS
    char *infile  = argv[1];
    char *outfile = argv[2];
    int   N       = atoi(argv[3]);
    char *F       = argv[4];

    RGB *image, *imageNew;
    int  width, height, max;
    int  AVERAGE   = (strcmp(F, "A") == 0) ? TRUE : FALSE;
    int  QUICKSORT = (N == 3) ? FALSE : TRUE; // bubble sort is faster for N=3

    image    = readPPM(infile, &width, &height, &max);
    imageNew = (RGB *)calloc(width * height, sizeof(RGB));

    // if true, do all the work myself
    if (NP == 1) {
      filter(N, width, height, AVERAGE, QUICKSORT, 0,
             width * height - 1, image, imageNew, 1);
      writePPM(outfile, width, height, max, imageNew);
      free(imageNew);
    } else {
      // number of rows to go through.
      // same for all processes except the last (which has a few more)
      int nr = floor(height / NP);

      // the "bulk". The number of the pixels that the process must return
      int b = nr * width;

      ih.N         = N;
      ih.NP        = NP;
      ih.width     = width;
      ih.height    = height;
      ih.AVERAGE   = AVERAGE;
      ih.QUICKSORT = QUICKSORT;
      int nm  = (N - 1) / 2;
      int wnm = width * nm;

      // index into the image where the whole slice (including oversend pixel
      // data) comes from
      int startSliceIndex = 0;
      int endSliceIndex   = 0;

      // first element is P0, last is PN-1
      int returnSizes[NP];

      // send the header + image slice info to [P1, PN-1]
      for (recvPin = 1; recvPin < NP; recvPin++) {
        if (recvPin < (NP - 1)) {
          startSliceIndex = recvPin * b - wnm;
          endSliceIndex   = (recvPin + 1) * b + wnm - 1;
          ih.numPixels    = endSliceIndex - startSliceIndex + 1;
          ih.startIndex   = wnm;
          ih.endIndex     = b + wnm - 1;
        } else {
          startSliceIndex = (NP - 1) * b - wnm;
          endSliceIndex   = width * height - 1;
          ih.numPixels    = endSliceIndex - startSliceIndex + 1;
          ih.startIndex   = wnm;
          ih.endIndex     = ih.numPixels  - 1;
        }

        returnSizes[recvPin] = SEND_AMT;
        MPI_Send(&ih,                     sizeof(image_header_t) / sizeof(int),
                 MPI_INT, recvPin, tag, MPI_COMM_WORLD);
        MPI_Send(image + startSliceIndex, ih.numPixels * sizeof(RGB),
                 MPI_CHAR, recvPin, tag, MPI_COMM_WORLD);
      }

      // this process, P0 also has to do some work processing the image
      RGB *slice, *sliceNew;
      nr              = floor(height / NP);
      b               = nr * width;
      returnSizes[0]  = b;
      startSliceIndex = 0;
      endSliceIndex   = b + wnm - 1;
      ih.numPixels    = endSliceIndex + 1;
      ih.startIndex   = 0;
      ih.endIndex     = b - 1;

      slice = (RGB *)calloc(ih.numPixels, sizeof(RGB));
      memcpy(slice, image, ih.numPixels * sizeof(RGB));

      sliceNew = (RGB *)calloc(SEND_AMT, sizeof(RGB));
      filter(N, width, height, AVERAGE, QUICKSORT,
             ih.startIndex, ih.endIndex, slice, sliceNew, 0);

      // receive and combine the image slices into one final image
      memcpy(imageNew, sliceNew, b * sizeof(RGB));
      int offset = 0;

      for (recvPin = 1; recvPin < NP; recvPin++) {
        // return sizes is -1 because we need
        // the offset of the last contribution
        offset += returnSizes[recvPin - 1];

        MPI_Recv(imageNew + offset, returnSizes[recvPin] * sizeof(RGB),
                 MPI_CHAR, recvPin, tag, MPI_COMM_WORLD, &status);
      }

      // save the final image
      writePPM(outfile, width, height, max, imageNew);
      free(image);
      free(imageNew);
      free(slice);
      free(sliceNew);
    }
  } else {
    // this is not a MASTER_PROCESS, just a boring old other process
    RGB *slice, *sliceNew;

    // receive the header, "ih",  that gets us a lot of useful infromation
    MPI_Recv(&ih, sizeof(image_header_t) / sizeof(int), MPI_INT,
             MASTER_PROCESS, tag, MPI_COMM_WORLD, &status);

    // we send less back than we receive because we were
    // oversent pixel data for the mask calculation
    slice    = (RGB *)calloc(ih.numPixels, sizeof(RGB));
    sliceNew = (RGB *)calloc(SEND_AMT, sizeof(RGB));

    // receive pixel data for the horizontal slice of
    // the image we get to work on
    MPI_Recv(slice, ih.numPixels * sizeof(RGB), MPI_CHAR, MASTER_PROCESS,
             tag, MPI_COMM_WORLD, &status);

    filter(ih.N, ih.width, ih.height, ih.AVERAGE, ih.QUICKSORT,
           ih.startIndex, ih.endIndex, slice, sliceNew,
           pin == (NP - 1) ? 1 : 0);

    MPI_Send(sliceNew, (SEND_AMT)*sizeof(RGB), MPI_CHAR,
             MASTER_PROCESS, tag, MPI_COMM_WORLD);

    free(slice);
    free(sliceNew);
  }

    MPI_Finalize();
  return 0;
}

