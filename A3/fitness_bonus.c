/*     Copyright (C) 2016  N. Perna, N. Nedialkov, T. Gwosdz

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

#include <math.h>
#include "a3_bonus.h"

// Given one RGB from the final image, we compare against "pop_size" number of Cells
void compFitnessCell(const RGB *desired_img, Cell *cell, int pop_size){
	int q=0;
	Cell *c;
	int r,g,b;
	for (q = 0; q < pop_size; q++) {
		c=cell+q;
		r=desired_img->r - c->rgb.r;
		g=desired_img->g - c->rgb.g;
		b=desired_img->b - c->rgb.b;
		c->fitness = (r*r + g*g + b*b);
		//c->fitness = r*150+g*150+b*150;
	}
}

inline double pixelDistance(const RGB *a, const RGB *b)
{
  int rd = a->r - b->r,
      gd = a->g - b->g,
      bd = a->b - b->b;

  return (double)(rd * rd + gd * gd + bd * bd);
}

void compFitness(const RGB *A, Individual *B, int wh_product)
{
  int i;
  double f = 0;

  for (i = 0; i < wh_product; i++){
	 double inter= pixelDistance(&A[i], &(B->image[i]));
	 f += inter;
  }

  B->fitness = f;
}

