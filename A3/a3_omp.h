#ifndef INCLUDED_A3_H
#define INCLUDED_A3_H

#ifdef _OPENACC
# include <openacc.h>
#endif // ifdef _OPENACC

#ifdef _OPENMP
# include <omp.h>
#endif // ifdef _OPENMP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct {
  unsigned char r,g,b;
} RGB;

typedef struct {
  RGB *image;
  double fitness;
} Individual;

// one cell is a pixel. Used for the bonus -DBONUS_EFF
// fitness is how far off it is from the true pixel value
typedef struct{
  RGB rgb;
  int fitness;
} Cell;

#define RANDOM(max) (rand() % (max + 1))
#define DO_NOT_CHANGE (500) // mutation rate
#define MON_GEN (100)       // monitor the output every N generations

RGB * readPPM(const char* file, int* width, int* height, int* max);
void  writePPM(const char* file, int width, int height, int max, const RGB *image);
RGB * randomImage(int width, int height, int max);
void  compFitness (const RGB *goal, Individual *curr, int wh_product);
void  mate (Individual *parent1, Individual *parent2, Individual *child1, Individual *child2, int wh_product);
void  mutate (Individual *curr, int wwh_product, int rate, int max);
void compImage(const RGB *desired_image, int width, int height, int max,
	       int num_generations, int population_size,
	       RGB *found_image, const char *output_file);
int * randomperm(int n);
void bonusVideo(int g, const char *output_file);
void monitor(int         g,
                    const char *output_file,
                    int         width,
                    int         height,
                    int         max,
                    Individual *ind,
                    double      prev_fitness,
                    double     *old_time,
                    double     *new_time);
#endif
