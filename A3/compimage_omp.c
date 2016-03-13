/*  Copyright (C) 2016  N. Perna, N. Nedialkov, T. Gwosdz

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "a3_omp.h"

static int fitnessCompare(const void *a, const void *b)
{
  return (*(Individual *)a).fitness - (*(Individual *)b).fitness;
}

void compImage(const RGB *desired_image, int width, int height, int max,
               int num_generations, int population_size,
               RGB *found_image, const char *output_file)
{
  int wh_product = width * height;
  int rate       = wh_product / DO_NOT_CHANGE;

#ifdef MONITOR
  double new_time = omp_get_wtime();
  double old_time = new_time;
#endif 

  //============================================================================
  Individual *population = (Individual *)
                           malloc(population_size * sizeof(Individual));
  assert(population);

  int i;
  for (i = 0; i < population_size; i++) population[i].image = randomImage(width,
                                                                          height,
                                                                          max);

  for (i = 0; i < population_size; i++) 
	compFitness(desired_image,population + i,wh_product);
 
  qsort(population, population_size, sizeof(Individual), fitnessCompare);

  //============================================================================
  int g;
#ifdef MONITOR
  double prev_fitness, current_fitness;
#endif
  for (g = 0; g < num_generations; g++)
  {
#ifdef MONITOR
    prev_fitness = population[0].fitness;
#endif

#pragma omp parallel for schedule(dynamic) default(none) shared(population_size, population, wh_product) private(i)
    for (i = 0; i < population_size / 2; i += 2)
      mate(population + i, population + i + 1,
           population + population_size / 2 + i,
           population + population_size / 2 + i + 1,
           wh_product);

    // after the first 1/4 individuals, each individual can mutate
    int mutation_start =  population_size / 4;

//#pragma omp parallel for// XXX This is a bad idea
    for (i = mutation_start; i < population_size; i++) mutate(population + i,
                                                              wh_product,
                                                              rate, max);

    //  Recompute fitness
#pragma omp parallel for schedule(dynamic) default(none) shared(population_size, desired_image, population, wh_product) private(i)
    for (i = 0; i < population_size; i++)
	compFitness(desired_image, population + i, wh_product);

    // Sort in non-decreasing fitness
    qsort(population, population_size, sizeof(Individual), fitnessCompare);

#ifdef MONITOR 
   monitor(g, output_file, width, height, max, &population[0], prev_fitness, &old_time, &new_time);
#endif 
  }
#ifdef MONITOR
   monitor(g, output_file, width, height, max, &population[0], prev_fitness, &old_time, &new_time);
#endif 
//==============================================================================

// Return the image that is found
memmove(found_image, population[0].image, wh_product * sizeof(RGB));

// release memory
for (i = 0; i < population_size; i++) free(population[i].image);
free(population);
}

