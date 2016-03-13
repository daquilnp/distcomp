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

#include <stdlib.h>
#include <assert.h>
#include "a3_bonus.h"

#ifdef VERSION_OMP
# define NUM_ARGS (1 + 5)
#else /* ifdef VERSION_OMP */
# define NUM_ARGS (1 + 4)
#endif /* ifdef VERSION_OMP */

int main(int argc, char **argv)
{
  if (argc != NUM_ARGS) {
    printf("Wrong number of arguments. Want[%d] Given[%d]\n", NUM_ARGS, argc);
    exit(-1);
  }

#ifdef VERSION_OMP
  omp_set_num_threads(atoi(argv[1]));
  const char *input_file  = argv[2];
  const char *output_file = argv[3];
  int num_generations     = atoi(argv[4]);
  int population_size     = atoi(argv[5]);
#else /* ifdef VERSION_OMP */
  const char *input_file  = argv[1];
  const char *output_file = argv[2];
  int num_generations     = atoi(argv[3]);
  int population_size     = atoi(argv[4]);
#endif /* ifdef VERSION_OMP */

  // population size, must be multiple of 4 right now
  assert(population_size % 4 == 0);

  // Bonus (5 points) Output intermediate images to show convergence
 #ifdef BONUS_VIDEO
  printf("bonus video...");
  char buf[64];
  bzero(buf, 64);
  sprintf(buf, "mkdir -p ./bonus_%s", output_file);
  int ret = system(buf);
  printf("buf[%s] [%d]\n", buf, ret);
#endif /* ifdef BONUS_VIDEO */

  // Load the desired image
  RGB *desired_image;
  int  width, height, max;
  desired_image = readPPM(input_file, &width, &height, &max);

  // Compute an image
  RGB *found_image = (RGB *)malloc(width * height * sizeof(RGB));
  
  double start_time = omp_get_wtime();
  compImage(desired_image, width, height, max,
            num_generations, population_size, found_image, output_file);
  double time = omp_get_wtime() - start_time;
  printf("time = [%f]s\n", time);

  // Write it back into an output file
  writePPM(output_file, width, height, max, found_image);

  free(found_image);
  free(desired_image);

  return 0;
}

