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

#include "a3_bonus.h"

void mutateCells(Cell *img, int pop_size, int width, int height, int max){
	int rate = width * height * pop_size / 500;
	int i,j;
	for (i = 0; i < rate; i++)
	  {
	    // Pick a pixel at random.
new_rand:
	    j = rand() % (width * height * pop_size);

	    // don't mutate the alpha male
            if(j % pop_size == 0)
		goto new_rand;

	    // and modify it
	    img[j].rgb.r = RANDOM(max);
	    img[j].rgb.g = RANDOM(max);
	    img[j].rgb.b = RANDOM(max);
	  }
}

void mutate(Individual *curr, int width, int height, int max)
{
  // Set how many pixels to mutate. The constant 500 is somewhat
  // random. You can experiment with different constants.
  int rate = width * height / 500;

  RGB *image = curr->image;
  int  i, j;

  for (i = 0; i < rate; i++)
  {
    // Pick a pixel at random.
    j = rand() % (width * height);

    // and modify it
    image[j].r = RANDOM(max);
    image[j].g = RANDOM(max);
    image[j].b = RANDOM(max);
  }
}

