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
  float *camera_position_changes_array,
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
  float *best_path_distances;
  int *pixel_position_width;
  int *pixel_position_height;
  float step_size = 0.01;
  // float straight[3] = {0,0,0};
  // float left_right[3] = {0,0,0};
  // float up_down[3] = {0,0,0};
  vec3 *distance_vector_points;
  vec3 *best_path_distance_vector_points;

  int pin_spacing = 5;
  int check_frame_path = 12;
  distance_to_pixel_array =  (float *)malloc(sizeof(float) * (width*height/pin_spacing));
  pixel_position_height =  (int *)malloc(sizeof(int) * (width*height/pin_spacing));
  pixel_position_width =  (int *)malloc(sizeof(int) * (width*height/pin_spacing));
  distance_vector_points =  (vec3 *)malloc(sizeof(vec3) * (width*height/pin_spacing));
  best_path_distance_vector_points =  (vec3 *)malloc(sizeof(vec3) * (width*height/(pin_spacing)));
  best_path_distances =  (float *)malloc(sizeof(float) * (width*height/(pin_spacing)));
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
    // printf("%f\n", distance_to_pixel );
	  // distance_to_pixel_array[j*width + i] = distance_to_pixel;
    if (pixel_count%pin_spacing == 0){
      distance_to_pixel_array[distance_to_pixel_index] = distance_to_pixel;
      pixel_position_width[distance_to_pixel_index] = i;
      pixel_position_height[distance_to_pixel_index] = j;
      distance_vector_points[distance_to_pixel_index] = test_vector;
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
      // printf("%d", distance_to_pixel_index);
      printProgress((j+1)/(float)height,getTime()-time);

    }

  int max_index = 0;
  int found_flag = 0;
  float current_max = 0;
  int current_max_position_width = 0;
  int current_max_position_height = 0;
  vec3 current_max_vector_point; 
  while (found_flag == 0){
    
      if (distance_to_pixel_array[max_index] < 0.02){
          current_max = distance_to_pixel_array[max_index];
          current_max_position_width = pixel_position_width[max_index];
          current_max_position_height = pixel_position_height[max_index];
          current_max_vector_point = distance_vector_points[max_index];
          found_flag = 1;

      }
      max_index++;
  }
  
  for (;max_index < distance_to_pixel_index; max_index++){
      if (distance_to_pixel_array[max_index] > current_max && distance_to_pixel_array[max_index] < 0.02){
          current_max = distance_to_pixel_array[max_index];
          current_max_position_width = pixel_position_width[max_index];
          current_max_position_height = pixel_position_height[max_index];
          current_max_vector_point = distance_vector_points[max_index];
         
      }

  }
  printf("furthest point at, x: %f y: %f z: %f\n",current_max_vector_point.x,current_max_vector_point.y,
    current_max_vector_point.z);
  vec3 current_min_vector_point = distance_vector_points[0];
  int min_index = 0;
  float current_min = 0;
  int current_min_position_width = 0;
  int current_min_position_height = 0;
  found_flag = 0;
    while (found_flag == 0){
      if (distance_to_pixel_array[min_index] < 0.02){
          current_min = distance_to_pixel_array[min_index];
          current_min_position_width = pixel_position_width[min_index];
          current_min_position_height = pixel_position_height[min_index];
          current_min_vector_point = distance_vector_points[min_index];
          found_flag = 1;
       

      }
       min_index ++;
       
  }
  for (; min_index < distance_to_pixel_index; min_index++){
      if (distance_to_pixel_array[min_index] < current_min && distance_to_pixel_array[min_index] < 0.02){
          current_min = distance_to_pixel_array[min_index];
          current_min_position_width = pixel_position_width[min_index];
          current_min_position_height = pixel_position_height[min_index];
          current_min_vector_point = distance_vector_points[min_index];
      }
  }
  printf("closest point at, x: %f y: %f z: %f\n",current_min_vector_point.x,current_min_vector_point.y,
    current_min_vector_point.z);

if (frame_no%check_frame_path == 0 || current_min <= 0.00003){  
    printf("\ncurrent min: %f\n", current_min);
    int clear_position_changes;
    for (clear_position_changes = 0; clear_position_changes < 3; clear_position_changes++){
    camera_position_changes_array[clear_position_changes] = 0;
    }
    for (clear_position_changes = 0; clear_position_changes < 3; clear_position_changes++){
    camera_angle_changes_array[clear_position_changes] = 0;
    }

  int on_left = 0;
  if (current_min_position_width <= width/2){
      on_left = 1;
  }
  int num_of_max_counter[5] = {0,0,0,0};
  int num_of_min_counter[5] = {0,0,0,0};

  for (min_index = 0; min_index < distance_to_pixel_index; min_index++){
    if(distance_to_pixel_array[min_index] < 0.02){
      if (pixel_position_width[min_index] <= (width/2) && pixel_position_height[min_index] <= (height/2) 
      && on_left !=1 && distance_to_pixel_array[min_index] > 1.2*current_min){
        num_of_min_counter[0] +=1;
      }
      else if (pixel_position_width[min_index] > (width/2) && pixel_position_height[min_index] <= (height/2)
      && on_left ==1 && distance_to_pixel_array[min_index] > 1.2*current_min){
        num_of_min_counter[1] +=1;
      }
      else if (pixel_position_width[min_index] <= (width/2) && pixel_position_height[min_index] > (height/2) 
      && on_left != 1 && distance_to_pixel_array[min_index] > 1.2*current_min){
        num_of_min_counter[2] += 1;
    }
      else if (pixel_position_width[min_index] > width/2 && pixel_position_height[min_index] > (height/2)
      && on_left == 1 && distance_to_pixel_array[min_index] > 1.2*current_min){
        num_of_min_counter[3] += 1;
      }
  }
}
  int worst_path = 0;
  int min_counter = num_of_min_counter[0];
  for (min_index = 1; min_index < 4; min_index++){
      if (num_of_min_counter[min_index] > min_counter){
        min_counter = num_of_min_counter[min_index];
        worst_path = min_index; 
      }

  }  

  for (max_index = 0; max_index < distance_to_pixel_index; max_index++){
    if(distance_to_pixel_array[max_index] < 0.02){
      if (worst_path != 0 && pixel_position_width[max_index] <= (width/2) && 
        pixel_position_height[max_index] <= (height/2) && on_left !=1 
        && distance_to_pixel_array[max_index] > 0.95*current_max ){
        num_of_max_counter[0] +=1;

      }
      else if (worst_path != 1 && pixel_position_width[max_index] > (width/2) && 
        pixel_position_height[max_index] <= (height/2)
      && on_left ==1 && distance_to_pixel_array[max_index] > 0.95*current_max){
        num_of_max_counter[1] +=1;
      }
      else if (worst_path != 2 &&
        pixel_position_width[max_index] <= (width/2) && pixel_position_height[max_index] > (height/2) 
      && on_left != 1 && distance_to_pixel_array[max_index] > 0.95*current_max){
        num_of_max_counter[2] += 1;
    }
      else if (worst_path != 3 &&
        pixel_position_width[max_index] > width/2 && pixel_position_height[max_index] > (height/2)
      && on_left == 1 && distance_to_pixel_array[max_index] > 0.95*current_max){
        num_of_max_counter[3] += 1;
      }

    }
  }
  int best_path = 0;
  int max_counter = num_of_max_counter[0];
  for (max_index = 1; max_index < 4; max_index++){
      if (num_of_max_counter[max_index] > max_counter){
        max_counter = num_of_max_counter[max_index];
        best_path = max_index; 

      }

  }  
  int element_counter = 0;
  for (max_index = 0; max_index < distance_to_pixel_index; max_index++){
    if(best_path_distances[max_index] < 0.02){
      if (best_path == 0 && pixel_position_width[max_index] <= (width/2) && 
        pixel_position_height[max_index] <= (height/2) && on_left !=1 
        && distance_to_pixel_array[max_index] > 0.95*current_max ){
          best_path_distance_vector_points[max_index] = distance_vector_points[max_index];
          best_path_distances[max_index] = distance_to_pixel_array[max_index];
          element_counter++;
      }
      else if (best_path == 1 && pixel_position_width[max_index] > (width/2) && 
        pixel_position_height[max_index] <= (height/2)
      && on_left ==1 && distance_to_pixel_array[max_index] > 0.95*current_max){
          best_path_distance_vector_points[max_index] = distance_vector_points[max_index];
          best_path_distances[max_index] = distance_to_pixel_array[max_index];
          element_counter++;
      }
      else if (best_path == 2 &&
        pixel_position_width[max_index] <= (width/2) && pixel_position_height[max_index] > (height/2) 
      && on_left != 1 && distance_to_pixel_array[max_index] > 0.95*current_max){
          best_path_distance_vector_points[max_index] = distance_vector_points[max_index];
          best_path_distances[max_index] = distance_to_pixel_array[max_index];
          element_counter++;
    }
      else if (best_path == 3 &&
        pixel_position_width[max_index] > width/2 && pixel_position_height[max_index] > (height/2)
      && on_left == 1 && distance_to_pixel_array[max_index] > 0.95*current_max){
          best_path_distance_vector_points[max_index] = distance_vector_points[max_index];
          best_path_distances[max_index] = distance_to_pixel_array[max_index];
          element_counter++;
      }

    }
  }

  float current_max = best_path_distances[0];
  vec3 current_max_vector_point = distance_vector_points[0];
  for (max_index = 1; max_index < distance_to_pixel_index; max_index++){
      if (best_path_distances[max_index] > current_max && best_path_distances[max_index] < 0.02){
          current_max = best_path_distances[max_index];
          current_max_vector_point = distance_vector_points[max_index];
      }

  }
  printf("NEW CAMERA ANGLE \n");
  camera_angle_array[0] = current_max_vector_point.x;
  camera_angle_array[1] = current_max_vector_point.y;
  camera_angle_array[2] = current_max_vector_point.z;


}







  float vector_distance = sqrtf(powf(camera_angle_array[0] -camera_position_array[0],2)
     + powf(camera_angle_array[1] -camera_position_array[1],2) + powf(camera_angle_array[3] -camera_position_array[3],2));
    float t = step_size/vector_distance;
  camera_position_array[0] += t*(camera_angle_array[0]-camera_position_array[0]);
  camera_position_array[1] += t*(camera_angle_array[1]-camera_position_array[1]);
  camera_position_array[2] += t*(camera_angle_array[2]-camera_position_array[2]);

  printf("\n rendering done:\n");


  free(distance_to_pixel_array);
  free(pixel_position_height);
  free(pixel_position_width);

    return 1;
}





