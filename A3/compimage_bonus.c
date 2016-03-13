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

#include "a3_bonus.h"

static int fitnessCompare(const void *a, const void *b)
{
  return (*(Individual *)a).fitness - (*(Individual *)b).fitness;
}

static int fitnessCompareCells(const void *a, const void *b)
{
  return (*(Cell *)a).fitness - (*(Cell *)b).fitness;
}

inline void bonusVideo(int g, const char *output_file) {
# define BUF_SIZE (512)
  char buf[BUF_SIZE];
  char output_file_basename_increment[BUF_SIZE / 4];
  int  numZeros = 9;
  char zeroFill[12];
  
  bzero(zeroFill, 12);
  bzero(buf,      BUF_SIZE);

  int count = 0;
  int newg  = g;

  for (; newg / 10 > 0; numZeros--) {
    newg = newg / 10;
  }

  for (; count < numZeros; count++) {
    zeroFill[count] = '0';
  }
  sprintf(output_file_basename_increment,
          "%s%d",
          zeroFill,
          g);
  sprintf(buf,
          "convert %s ./bonus_%s/%s.png",
          output_file,
          output_file,
          output_file_basename_increment);
  printf("buf[%s]\n", buf);
  int ret = system(buf);
}

#define vid(){\
    for (q = 0; q < wh_product*population_size; q+=population_size){\
		memcpy(&skim_img[q/population_size], &(img[q].rgb), sizeof(RGB)); \
  	}\
	writePPM(output_file,\
             width,\
             height,\
             max,\
             skim_img);\
	bonusVideo(g, output_file);\
}

void compImage(const RGB *desired_image, int width, int height, int max,
               int num_generations, int population_size,
               RGB *found_image, const char *output_file)
{
  int wh_product = width * height;
  int whpop      = wh_product*population_size;
  int rate       = wh_product / DO_NOT_CHANGE;

#ifdef MONITOR
  double new_time = omp_get_wtime();
  double old_time = new_time;
#endif 

  printf("width[%d] height[%d] pop[%d] max[%d]\n", width, height, population_size, max);

  int v=0;
  int g=0;
  int q=0;
  Cell * img = randomCells(width, height, population_size, max); // pop_size cells per pixel
  RGB *skim_img = randomImage(width,height,max);// just to allocate memory

  g=0;
  vid();     

  for (g = 1; g <= num_generations; g++){

	 /*
          if(g == 1000){
		// change the picture
		free(desired_image);
		desired_image = readPPM("ultron.ppm", &width, &height, &max);
	  }*/

	  for (q = 0; q < whpop; q+=population_size)   {
		// if you remove mating, you get much faster reactions to disturbances
		mateCells(img+q, population_size); 
	  }

	  mutateCells(img, population_size, width, height, max); 
 
         for (q = 0; q < wh_product; q++)          
 		compFitnessCell(desired_image+q, img+(q*population_size), population_size);

#pragma omp parallel for
	for (q = 0; q < whpop; q+=population_size)
		qsort(img+q, population_size, sizeof(Cell), fitnessCompareCells);

    if(g % MON_GEN == 0 || g == 1){
     	vid(); 
    }

  }// end of generations

  for (q = 0; q < wh_product*population_size; q+=population_size){
	memcpy(&skim_img[q/population_size], &(img[q].rgb), sizeof(RGB)); 
  }
  memmove(found_image, skim_img, wh_product * sizeof(RGB));
  free(skim_img);
}

