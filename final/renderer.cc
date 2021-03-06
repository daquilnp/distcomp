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
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"
#include <math.h>


extern float getTime();
extern void   printProgress( float perc, float time );

extern float rayMarch (const RenderParams &render_params, const vec3 &from, const vec3  &to, float eps, pixelData &pix_data,
  vec3 &vector_of_pixel);
extern vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
          const vec3 &from, const vec3  &direction);

int renderFractal(const CameraParams &camera_params, float *camera_position_array,
  float *camera_position_changes_array, int move_position,
float *camera_angle_array, float *camera_angle_changes_array, int frame_no,
  const RenderParams &renderer_params, unsigned char* image)
{
  const float eps = powf(10.0, renderer_params.detail); 
  float farPoint[3];
  vec3 to, from, test_vector;
  
  from.SetfloatPoint(camera_params.camPos);
  
  const int height = renderer_params.height;
  const int width  = renderer_params.width;

  
  float *distance_to_pixel_array;
  float *distance_to_point_array;
  vec3 *distance_vector_points;
  float step_size = 0.01;
 

  int pin_spacing = 5;
  // int check_frame_path = 30;
  distance_to_pixel_array =  (float *)malloc(sizeof(float) * (width*height/pin_spacing));
  distance_to_point_array =  (float *)malloc(sizeof(float) * (width*height/pin_spacing));
  distance_vector_points =  (vec3 *)malloc(sizeof(vec3) * (width*height/pin_spacing));

  pixelData pix_data;
  
  float time = getTime();
  vec3 color;
  
  int i,j,k;
  int pixel_count = 0;
  int distance_to_pixel_index = 0;

  for(j = 0; j < height; j++)
    {
      //for each column pixel in the row
      for(i = 0; i <width; i++)
  {
      // get point on the 'far' plane
      // since we render one frame only, we can use the more specialized method
      UnProject(i, j, camera_params, farPoint);


      // to = farPoint - camera_params.camPos
      to = Subtractfloatfloat(farPoint,camera_params.camPos);
      
      to.Normalize();
      vec3 test_vector;
      //render the pixel
      float distance_to_pixel = rayMarch(renderer_params, from, to, eps, pix_data, test_vector);

      if (i%pin_spacing == 0 && j%pin_spacing == 0){

        distance_to_pixel_array[distance_to_pixel_index] = distance_to_pixel;
        distance_vector_points[distance_to_pixel_index] = pix_data.hit;
        float new_distance = sqrtf(powf(pix_data.hit.x -camera_position_array[0],2)
       + powf(pix_data.hit.y-camera_position_array[1],2) + powf(pix_data.hit.z -camera_position_array[3],2));
        distance_to_point_array[distance_to_pixel_index] = new_distance;

        distance_to_pixel_index++; 

    }

      //get the colour at this pixel
      color = getColour(pix_data, renderer_params, from, to);
        
      //save colour into texture
      k = (j * width + i)*3;
      image[k+2] = (unsigned char)(color.x * 255);
      image[k+1] = (unsigned char)(color.y * 255);
      image[k]   = (unsigned char)(color.z * 255);
      pixel_count++;
  }
      printProgress((j+1)/(float)height,getTime()-time);

    }

if (move_position == 1){
  float vector_distance = sqrtf(powf(camera_angle_array[0] -camera_position_array[0],2)
     + powf(camera_angle_array[1] -camera_position_array[1],2) + powf(camera_angle_array[3] -camera_position_array[3],2));

  float t = step_size/vector_distance;
  camera_position_array[0] += t*(camera_angle_array[0]-camera_position_array[0]);
  camera_position_array[1] += t*(camera_angle_array[1]-camera_position_array[1]);
  camera_position_array[2] += t*(camera_angle_array[2]-camera_position_array[2]);
 
    if ( vector_distance < 0.4 || frame_no == 0){
      move_position = 0;
      int max_index = 0;
      int found_flag = 0;
      float current_max = 0;
      float current_max_distance = 0;
      vec3 current_max_vector_point; 
      while (found_flag == 0){
        
          if (distance_to_pixel_array[max_index] < 0.0002){
              current_max = distance_to_pixel_array[max_index];
              current_max_vector_point = distance_vector_points[max_index];
              current_max_distance = distance_to_point_array[max_index];
              found_flag = 1;
          }
          max_index++;
      }
      
      for (;max_index < distance_to_pixel_index; max_index++){
          if (distance_to_pixel_array[max_index] > current_max && distance_to_pixel_array[max_index] < 0.0002){
              current_max = distance_to_pixel_array[max_index];
              current_max_vector_point = distance_vector_points[max_index];
              current_max_distance = distance_to_point_array[max_index];        
          }

      }
      camera_angle_changes_array[0] = current_max_vector_point.x;
      camera_angle_changes_array[1] = current_max_vector_point.y;
      camera_angle_changes_array[2] = current_max_vector_point.z;
      
      

    }

}
else
{
float vector_distance = sqrtf(powf(camera_angle_changes_array[0] -camera_angle_array[0],2)
     + powf(camera_angle_changes_array[1] -camera_angle_array[1],2) 
     + powf(camera_angle_changes_array[3] -camera_angle_array[3],2));

  float t = 0.10;//*(step_size/vector_distance);
  if (vector_distance < 0.05 ){
    camera_angle_array[0] = camera_angle_changes_array[0];
    camera_angle_array[1] = camera_angle_changes_array[1];
    camera_angle_array[2] = camera_angle_changes_array[2];

    move_position = 1;
    
  }
  camera_angle_array[0] += t*(camera_angle_changes_array[0]-camera_angle_array[0]);
  camera_angle_array[1] += t*(camera_angle_changes_array[1]-camera_angle_array[1]);
  camera_angle_array[2] += t*(camera_angle_changes_array[2]-camera_angle_array[2]);

 
}



  printf("\n rendering done:\n");


  free(distance_to_pixel_array);
  free(distance_to_point_array);


    return move_position;
}





