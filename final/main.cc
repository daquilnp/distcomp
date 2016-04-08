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
#include <strings.h>//bzero
#include <math.h>
#include "vector3d.h"
    #include "color.h"

//ifdef _OPENACC
#include <openacc.h>
//#endif
#define PRINT_FRAMES (100) //make sure to change NUM_OF_SECTIONS value in renderer.cc to same value

void getParameters(char *filename, CameraParams *camera_params, 
  RenderParams *renderer_params, MandelBoxParams *mandelBox_params,
  float *camera_position_array, vec3 *frame_position_array, 
  float *camera_angle_array, vec3 *frame_angle_array, int frame_no);
void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
int renderFractal(const CameraParams camera_params, const RenderParams renderer_params, 
  unsigned char* image, const MandelBoxParams mandelBox_params, float *farPoint, vec3 *to, pixelData *pix, vec3* col, 
  const int width, const int height, const int area, const float eps,
  float *camera_position_array,
  float *camera_angle_array, int frame_no, int frame_array_index,
  vec3 *frame_position_array, vec3 *frame_angle_array);//, const float &eps);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);
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
  vec3 frame_position_array[500];
  vec3 frame_angle_array[500];
  
  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params,
    camera_position_array, frame_position_array, camera_angle_array, 
      frame_angle_array, frame_counter);

  int image_size = renderer_params.width * renderer_params.height;
  int image_memory = 3*image_size*sizeof(unsigned char);
  unsigned char *image = (unsigned char*)malloc(image_memory);

  const float eps = pow((float)10.0, renderer_params.detail); 
  const int height = renderer_params.height;
  const int width  = renderer_params.width;
  const int area   = width * height;

  float *farPoint = (float*) malloc(sizeof(float)*3*area);
  vec3 *to = (vec3*) malloc(sizeof(vec3) * area);
  pixelData *pix = (pixelData*) malloc(sizeof(pixelData) * area);
  vec3* col = (vec3*) malloc(sizeof(vec3) * area);

  char name[64];
  bzero(name,64);

  // int x=0;

  #pragma acc data copyin(renderer_params, mandelBox_params)
  {
    for(;frame_counter<1100;frame_counter++){
    if (frame_counter == (PRINT_FRAMES+1)){

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
      frame_angle_array[1].x, frame_angle_array[1].y, frame_angle_array[1].z); 
  }
  else{

    Next_Parameters(&camera_params, camera_position_array, camera_angle_array);
  }
  // printf("%f\n", camera_position_array[0]);

      init3D(&camera_params, &renderer_params);
    next_frame_flag =  renderFractal(camera_params, renderer_params, image, mandelBox_params, 
        farPoint, to, pix, col, width, height, area, eps,
        camera_position_array,
    camera_angle_array, frame_counter, frame_array_index
    , frame_position_array, frame_angle_array);
  if (next_frame_flag == 1){
      frame_array_index++;
      next_frame_flag = 0;
      printf("\nNEXT FRAME IN ARRAY\n");

  }
   if (frame_counter > PRINT_FRAMES){
      sprintf(name, "image-%05d.bmp", frame_counter);
      saveBMP(name, image, renderer_params.width, renderer_params.height); //renderer_params.file_name
      printf("\nWriting to: %s\n", name);
    }
}
}
  free(image);

  return 0;

}
