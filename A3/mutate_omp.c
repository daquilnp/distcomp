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

#include "a3_omp.h"

void mutate(Individual *curr, int wh_product, int rate, int max)
{
  RGB *image = curr->image;
  int  i, j;

  for (i = 0; i < rate; i++)
  {
    // Pick a pixel at random.
    j = rand() % (wh_product);

    // and modify it
    image[j].r = RANDOM(max);
    image[j].g = RANDOM(max);
    image[j].b = RANDOM(max);
  }
}

