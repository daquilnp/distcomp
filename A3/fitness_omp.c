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
#include "a3_omp.h"

#define sft(a)  (a<<((a & 0x80) ? 8 : 0))
#define RET(a,b,c) (double)((a<<8) + (b<<4) + (c<<2))

inline int pixelDistance(const RGB *a, const RGB *b)
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

