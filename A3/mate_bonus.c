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

void mateCells(Cell *pop, int pop_size){
	int i=0;
	int pop2 = pop_size / 2;
	for (i = 0; i < pop2; i += 2){
		// cross over at the rgb
		int crossover = RANDOM(1); 
		Cell *p1=pop+i;
		Cell *p2=pop+i+1;
		Cell *c1=pop+i+pop2;
		Cell *c2=pop+i+pop2 + 1;
	
		if(crossover == 0){
			c1->rgb.r = p1->rgb.r;
			c1->rgb.g = p2->rgb.g;
			c1->rgb.b = p2->rgb.b;
			c2->rgb.r = p2->rgb.r;
			c2->rgb.g = p1->rgb.g;
			c2->rgb.b = p1->rgb.b;
		}else if(crossover == 1){
			c1->rgb.r = p1->rgb.r;
			c1->rgb.g = p1->rgb.g;
			c1->rgb.b = p2->rgb.b;
			c2->rgb.r = p2->rgb.r;
			c2->rgb.g = p2->rgb.g;
			c2->rgb.b = p1->rgb.b;
		}else{
			printf("ERROR MATE\n");
		}
	}
}

void mate(Individual *parent1, Individual *parent2,
          Individual *child1, Individual *child2, int wh_product)
{
  int crossover = RANDOM(wh_product - 1);
  int i;
  int y;
//#pragma omp sections //nowait // try num_threads(2), it may be faster than getting every thread to go in parallel, but then again, the nowait means they don't wait for the other one to finish. if we have 4 threads then 2 will (redundantly) do the first loop and 2 the second with an implied barrier at the end, but not if we do "nowait"
{
//*
//#pragma omp section
{
  for (i = 0; i < crossover; i++)
  {
    child1->image[i] = parent1->image[i];
    child2->image[i] = parent2->image[i];
  }
}
//#pragma omp section
{
  for (y = crossover; y < wh_product; y++)
  {
    child1->image[y] = parent2->image[y];
    child2->image[y] = parent1->image[y]; 
  }
}
//*/
///*
/*
//#pragma omp section
{
  for (i = 0; i < crossover/2; i++)
  {
    child1->image[i] = parent1->image[i];
    child2->image[i] = parent2->image[i];
  }
}
//#pragma omp section
{
  for (i = crossover/2; i < crossover; i++)
  {
    child1->image[i] = parent1->image[i];
    child2->image[i] = parent2->image[i];
  }

}
//#pragma omp section
{
  for (i = crossover; i < (wh_product-crossover)/2; i++)
  {
    child1->image[i] = parent2->image[i];
    child2->image[i] = parent1->image[i]; 
  }
}
//#pragma omp section
{
  for (i = crossover + (wh_product-crossover)/2; i < wh_product; i++)
  {
    child1->image[i] = parent2->image[i];
    child2->image[i] = parent1->image[i]; 
  }
}
*/
}
}

