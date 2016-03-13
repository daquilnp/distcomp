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
#ifdef _OPENACC
#include <openacc.h>
#endif

#include <math.h>
#include "a3_acc.h"

inline double pixelDistance (const RGB *a, const RGB *b)
{
  int rd = a->r-b->r,
    gd = a->g-b->g,
    bd = a->b-b->b;
  
  return (double)(rd*rd+gd*gd+bd*bd);
}


//void compFitness (const RGB *A, Individual *B, int width, int height) 
void compFitness (const RGB *A, const RGB *B, double *fit,  int width, int height) 
{ 

#pragma acc kernels present(A[0:width*height]) pcopyin(B[0:width*height]) pcopy(fit[0:1])
  {
    int i;
    double f = 0;

  #pragma acc loop
    for (i = 0; i < width * height; i++) 
      f += pixelDistance(&A[i], &B[i]);
   
    *fit = f;
  }
}




