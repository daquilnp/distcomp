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
#include <stdlib.h>
#include <stdio.h>
#include "camera.h"
#include "renderer.h"
#include "mandelbox.h"
#include "vector3d.h"

void getParameters(char *filename, float *camera_position_array, vec3 *frame_position_array, 
  float *camera_angle_array, vec3 *frame_angle_array, int frame_no,
CameraParams *camera_params,
 RenderParams *renderer_params, MandelBoxParams *mandelBox_params);
void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
int renderFractal(const CameraParams &camera_params, float *camera_position_array,
float *camera_angle_array, int frame_no, int frame_array_index,
vec3 *frame_position_array, vec3 *frame_angle_array,
  const RenderParams &renderer_params, unsigned char* image,
 MandelBoxParams &mandelBox_params);

void saveBMP      (const char* filename, int frame_no, const unsigned char* image, int width, int height);

void Next_Parameters(CameraParams *camera_params, float *camera_position_array, float *camera_angle_array)
{
  float *pos;
  float *angle;
  pos = camera_params->camPos;
  *pos = camera_position_array[0];
  *(pos+1) = camera_position_array[1];
  *(pos+2) = camera_position_array[2];
  printf("camPos: %f %f %f\n", *pos, *(pos+1) , *(pos+2));
  angle = camera_params->camTarget;
  *angle = camera_angle_array[0];
  *(angle+1) = camera_angle_array[1];
  *(angle+2) = camera_angle_array[2];
  printf("camAng: %f %f %f\n", *angle, *(angle+1) , *(angle+2));

}
void Next_Parameters_Vector(CameraParams *camera_params, vec3 camera_position_array, vec3 camera_angle_array)
{
  float *pos;
  float *angle;
  pos = camera_params->camPos;
  *pos = camera_position_array.x;
  *(pos+1) = camera_position_array.y;
  *(pos+2) = camera_position_array.z;
  printf("camPos: %f %f %f\n", *pos, *(pos+1) , *(pos+2));
  angle = camera_params->camTarget;
  *angle = camera_angle_array.x;
  *(angle+1) = camera_angle_array.y;
  *(angle+2) = camera_angle_array.z;
  printf("camAng: %f %f %f\n", *angle, *(angle+1) , *(angle+2));

}

int main(int argc, char** argv)
{
  MandelBoxParams mandelBox_params;
  CameraParams    camera_params;
  RenderParams    renderer_params;
  float camera_position_array[3];

  float camera_angle_array[3];
  int frame_counter = 0;
  int frame_array_index = 1;
  int next_frame_flag = 0;
  vec3 frame_position_array[30];
  vec3 frame_angle_array[30];
 
  // int clear_position_changes;
  while (frame_counter < 400){

  if (frame_counter == 0){
    getParameters(argv[1], camera_position_array, frame_position_array, camera_angle_array, 
      frame_angle_array, frame_counter, 
     &camera_params, &renderer_params, &mandelBox_params);

   // Get_Initial_Vector(&camera_params, frame_position_array[0], frame_angle_array[0]);

    // printf("camAng: %f %f %f\n", *angle, *(angle+1) , *(angle+2));
  }
  else if (frame_counter == 6){

    camera_position_array[0] = frame_position_array[0].x;
    camera_position_array[1] = frame_position_array[0].y;
    camera_position_array[2] = frame_position_array[0].z;
    camera_angle_array[0] = frame_angle_array[0].x;
    camera_angle_array[1] = frame_angle_array[0].y;
    camera_angle_array[2] = frame_angle_array[0].z;
    Next_Parameters_Vector(&camera_params, frame_position_array[0], frame_angle_array[0]);

    printf("\nStarting Prints\n");
    printf("From %f %f %f TO %f %f %f\n", 
      frame_position_array[0].x, frame_position_array[0].y, frame_position_array[0].z,
      frame_angle_array[0].x, frame_angle_array[0].y, frame_angle_array[0].z); 
  }
  else{

    Next_Parameters(&camera_params, camera_position_array, camera_angle_array);
  }
  // printf("%f\n", camera_position_array[0]);
  int image_size = renderer_params.width * renderer_params.height;
  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));

  init3D(&camera_params, &renderer_params);


  next_frame_flag = renderFractal(camera_params, camera_position_array,
    camera_angle_array, frame_counter, frame_array_index
    , frame_position_array, frame_angle_array,
    renderer_params, image, mandelBox_params);
  if (next_frame_flag == 1){
      frame_array_index++;
      next_frame_flag = 0;
      printf("\nNEXT FRAME IN ARRAY\n");

  }
  if (frame_counter > 5){
    saveBMP(renderer_params.file_name , frame_counter, image, renderer_params.width, renderer_params.height);  
  }
  
  
  free(image);
  frame_counter++;
  }
  return 0;
}
