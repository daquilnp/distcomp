/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015-2016 at McMaster University.

    Copyright (C) 2015-2016 T. Gwosdz and N. Nedialkov

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
#include <assert.h>
#include <algorithm>
#include <stdio.h>

#include "color.h"
#include "renderer.h"
#include "mandelboxde.h"

extern float DE(const vec3 &p, MandelBoxParams &mandelBox_params);

static inline void normal(const vec3 & p, vec3 & normal, MandelBoxParams &mandelBox_params)
{
  // compute the normal at p
  const float sqrt_mach_eps = 3.4527e-04;// 1.4901e-08;

  float eps = std::max( MAGNITUDE_RET(p), 1.0 )*sqrt_mach_eps;

  vec3 e1 = {eps, 0,   0};
  vec3 e2 = {0  , eps, 0};
  vec3 e3 = {0  , 0, eps};

  vec3 ppe1 = PLUS(p, e1);
  vec3 pme1 = SUB(p, e1);
  vec3 ppe2 = PLUS(p, e2);
  vec3 pme2 = SUB(p, e2);
  vec3 ppe3 = PLUS(p, e3);
  vec3 pme3 = SUB(p, e3);

  normal = {DE(ppe1,mandelBox_params)-DE(pme1,mandelBox_params), DE(ppe2,mandelBox_params)-DE(pme2,mandelBox_params), DE(ppe3,mandelBox_params)-DE(pme3,mandelBox_params)};
  
  NORMALIZE(normal);
}

void rayMarch(const RenderParams &render_params, const vec3 &from, const vec3  &direction, float eps, pixelData& pix_data
, MandelBoxParams &mandelBox_params)
{

  float dist = 0.0;
  float totalDist = 0.0;
  
  // We will adjust the minimum distance based on the current zoom

  float epsModified = 0.0;
  
  int steps=0;
  vec3 p;
  do 
    {      
      //p = from + direction * totalDist;
      p = MULK(direction, totalDist);
      p = PLUS(p, from);
      dist = DE(p,mandelBox_params);
      
      totalDist += .95*dist;
      
      epsModified = totalDist;
      epsModified*=eps;
      steps++;
    }
  while (dist > epsModified && totalDist <= render_params.maxDistance && steps < render_params.maxRaySteps);
  
  //vec3 hitNormal; //XXX KA not used?
  if (dist < epsModified) 
    {
      //we didnt escape
      pix_data.escaped = false;
      
      // We hit something, or reached MaxRaySteps
      pix_data.hit = p;
      
      //figure out the normal of the surface at this point
      vec3 normPos = MULK(direction, epsModified); // XXX was const
      normPos = SUB(p, normPos);
      normal(normPos, pix_data.normal, mandelBox_params);
    }
  else 
    //we have the background colour
    pix_data.escaped = true;
// return dist;
}
