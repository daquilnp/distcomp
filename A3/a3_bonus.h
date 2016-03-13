/*
   Copyright (C) 2016  N. Perna, N. Nedialkov, T. Gwosdz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef INCLUDED_A3_H
#define INCLUDED_A3_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include <omp.h>

typedef struct {
  unsigned char r, g, b;
} RGB;

typedef struct {
  RGB   *image;
  double fitness;
} Individual;

#ifdef BONUS_EFF
// One cell is a pixel. Its fitness is how far off it is from the true pixel value
typedef struct{
  RGB rgb;
  int fitness;
} Cell;
#endif

#define DO_NOT_CHANGE (500) // mutation rate
#define MON_GEN (50)        // was 300
#define RANDOM(max) (rand() % (max + 1))

// #define RANDOM(max) (rand() & 0x00FF)

#define D() {printf("[%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__); fflush(stdout);} 

RGB* readPPM(const char *file,
             int        *width,
             int        *height,
             int        *max);
void writePPM(const char *file,
              int         width,
              int         height,
              int         max,
              const RGB  *image);

Cell* randomCells(int width, int height, int pop_size, int max);
RGB* randomImage(int width,
                 int height,
                 int max);

void compFitnessCell(const RGB *desired_img, Cell *cell, int pop_size);

void compFitness(const RGB  *goal,
                 Individual *curr,
                 int         wh_product);

void mateCells(Cell *pop, int pop_size);

void mate(Individual *parent1,
          Individual *parent2,
          Individual *child1,
          Individual *child2,
          int         wh_product);

void mutateCells(Cell *img, int pop_size, int width, int height, int max);

void mutate(Individual *curr,
            int         width,
            int         height,
            int         max);

void compImage(const RGB  *desired_image,
               int         width,
               int         height,
               int         max,
               int         num_generations,
               int         population_size,
               RGB        *found_image,
               const char *output_file);

int* randomperm(int n);

#endif // ifndef INCLUDED_A3_H
