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

#include "omp.h"

#ifdef _OPENACC
#include <openacc.h>
#endif

//#include <iostream>

#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"
#include "getcolor.h"

#define TURN_SMOOTHNESS (2) //closer to one means less smooth but faster
#define CHANGE_DIRECTION_THRESHOLD (0.015) //how close camera will get to the point it goes toward, smaller means closer
#define STEP_SIZE (0.01) //increment in each direction, larger means faster (keep relatively small <0.5)
#define PIN_SAMPLE_SIZE (10) //sampling of pixel. Larger means less pixels sampled
#define MAX_DISTANCE_THRESHOLD (0.8) //Largest distance to a pixel that the auto path will look for
// #define MIN_DISTANCE_THRESHOLD (0.0005) //If any vector in the sampled points is below this number, camera angle changes!
#define SNAP_ANGLE_DISTANCE (0.02) //when the camera angle is changing and hits this distance away from new point it will snap to it
#define NUM_OF_SECTIONS (100) //amount of stops, make sure to change PRINT_FRAMES in main.cc to same number 
#define POINT_JUMP (0.8) //between 0-1 how close the camera gets to the next stop. 1 means it will be right at the stop
#pragma acc routine seq  // XXX KA 
extern int UnProject(float winX, float winY, const CameraParams &camP, float *obj);

extern float getTime();
extern void   printProgress( float perc, float time );

#pragma acc routine seq
extern void rayMarch (const RenderParams &render_params, const vec3 &from, const vec3  &to, float eps, pixelData &pix_data, const MandelBoxParams &mandelBox_params);
inline float Vector_Distance(float *point1, vec3 point2){
  float distance = sqrtf(powf(point2.x -point1[0],2)
       + powf(point2.y-point1[1],2) + powf(point2.z-point1[2],2));
  return distance;
}
void Compute_Next_Stop(float t, float *initial, float *final){ //for computing next stop on pre-planned path
  initial[0] += t*(final[0] - initial[0]);
  initial[1] += t*(final[1] - initial[1]);
  initial[2] += t*(final[2] - initial[2]);

}
//for computing next step when interoplating between two stops
void Compute_Next_Increment(float t, float *initial, vec3 initial_vector, vec3 final_vector){ 
  initial[0] += t*(final_vector.x - initial_vector.x);
  initial[1] += t*(final_vector.y - initial_vector.y);
  initial[2] += t*(final_vector.z - initial_vector.z);

}

//for calculating max point at each stop to compute best path
vec3 Get_Max_Point(float *camera_position_array, pixelData *pix, int area){

    int max_index = 0;
    int found_flag = 0;
    float current_max = 0;
    // float current_max_distance = 0;
    vec3 current_max_vector_point; 

    while (found_flag == 0){
        float new_distance = Vector_Distance(camera_position_array, pix[max_index].hit);
       //  printf ("\nd: %f AT %f %f %f\n", new_distance,
       // pix[max_index].hit.x, pix[max_index].hit.y, pix[max_index].hit.z);                 
        if (new_distance < MAX_DISTANCE_THRESHOLD && max_index%PIN_SAMPLE_SIZE == 0
          && pix[max_index].hit.x >0.008 && pix[max_index].hit.y > 0.008 && pix[max_index].hit.z > 0.008){
            current_max = new_distance;
            current_max_vector_point = pix[max_index].hit;
 
            printf ("\nIndex: %d, Potential MAX DISTANCE: %f AT %f %f %f\n", max_index, current_max,
             current_max_vector_point.x, current_max_vector_point.y, current_max_vector_point.z); 
            found_flag = 1;
        }
        max_index++;
    }
    for(; max_index < area; max_index++){
      float new_distance = Vector_Distance(camera_position_array, pix[max_index].hit);
      if (max_index%PIN_SAMPLE_SIZE == 0 && new_distance < MAX_DISTANCE_THRESHOLD && new_distance > current_max
        && pix[max_index].hit.x >0.008 && pix[max_index].hit.y > 0.008 && pix[max_index].hit.z > 0.008){ 
           // printf ("\n(0 0 0): Location: %d %d,  d: %f AT %f %f %f\n", i, j, new_distance,
           // pix_data.hit.x, pix_data.hit.y, pix_data.hit.z);          
        current_max = new_distance;
        current_max_vector_point = pix[max_index].hit;
        printf ("\nIndex: %d, Potential MAX DISTANCE: %f AT %f %f %f\n", max_index, current_max,
        current_max_vector_point.x, current_max_vector_point.y, current_max_vector_point.z); 

        }
    }
     
    return current_max_vector_point;
}

int renderFractal(const CameraParams camera_params, 
  const RenderParams renderer_params, 
  unsigned char* image, 
  const MandelBoxParams mandelBox_params, float *farPoint, vec3 *to, pixelData *pix, vec3* col, const int width, const int height, 
  const int area, const float eps,
  float *camera_position_array,
float *camera_angle_array, int frame_no, int frame_array_index,
vec3 *frame_position_array, vec3 *frame_angle_array)
{
  vec3 from;
  SET_POINT(from,camera_params.camPos)
  // float *distance_to_point_array;
  // vec3 *distance_vector_points;

  // distance_to_point_array =  (float *)malloc(sizeof(float) * (width*height/PIN_SAMPLE_SIZE));
  // distance_vector_points =  (vec3 *)malloc(sizeof(vec3) * area);
   // int distance_to_point_index = 0; 
   int next_frame_flag = 0;
  
#pragma acc data copyout(image[0:3*area],pix[0:area]) create(to[0:area],col[0:area]) copyin(eps,from,camera_params) pcopyin(renderer_params,mandelBox_params)//,eps)
{
#pragma acc parallel loop //#pragma acc kernels
{
#pragma acc loop independent //#pragma acc for independent
  for(int i = 0; i < area; i++){
	  UnProject(i%width, i/width, camera_params, farPoint+i*3);//&farPoint[i*3]); // XXX KA 
    
	  VECESUBDUBDUB(to[i],farPoint,camera_params.camPos);
	  NORMALIZE(to[i]);
	  rayMarch(renderer_params, from, to[i], eps, pix[i], mandelBox_params);

	  getColour(pix[i], renderer_params, from, to[i], &col[i]);

	  //save colour into texture
	  image[i*3+2] = (unsigned char)(col[i].x * 255);
	  image[i*3+1] = (unsigned char)(col[i].y * 255);
	  image[i*3]   = (unsigned char)(col[i].z * 255);
	} // inner for
      //printProgress((j+1)/(float)height,getTime()-time);
    //}//end of outer for
}
}

if (frame_no < NUM_OF_SECTIONS){
    // printf("\nentering max finder\n");
    vec3 best_path_vector = Get_Max_Point(camera_position_array, pix, area);
    vec3 camera_position_vector; 

    camera_angle_array[0] = best_path_vector.x;
    camera_angle_array[1] = best_path_vector.y;
    camera_angle_array[2] = best_path_vector.z;
    float t_pos = POINT_JUMP;
    Compute_Next_Stop(t_pos, camera_position_array, camera_angle_array);

    camera_position_vector.x = camera_position_array[0];
    camera_position_vector.y = camera_position_array[1];
    camera_position_vector.z = camera_position_array[2];

    frame_angle_array[frame_no+1] = best_path_vector;
    frame_position_array[frame_no+1] = camera_position_vector;

     printf("\n Found Section:%d\n", (frame_no+1));


}
else{

    float  pos_vector_distance = Vector_Distance(camera_position_array, frame_position_array[frame_array_index]);
    // printf ("\nCurrent Distance: %f AT %f %f %f FROM %f %f %f\n", pos_vector_distance,
    //    camera_angle_array[0], camera_angle_array[1], camera_angle_array[2],
    //    camera_position_array[0], camera_position_array[1], camera_position_array[2]);
    printf ("\nCamera distance: %f\n",pos_vector_distance);
    if (pos_vector_distance < CHANGE_DIRECTION_THRESHOLD){
      // printf("\nChange Direction: distance = %f \n",  pos_vector_distance);  

      //if its below the threshold then it reached the next stop, we the next_frame_flag will be triggered
      camera_position_array[0] = frame_position_array[frame_array_index].x; 
      camera_position_array[1] = frame_position_array[frame_array_index].y;
      camera_position_array[2] = frame_position_array[frame_array_index].z;
      printf("\nSnapped In Camera Position\n");
      next_frame_flag = 1;
     
      // printf ("\nNEW MAX DISTANCE: %f AT %f %f %f\n", current_max,
      //  current_max_vector_point.x, current_max_vector_point.y, current_max_vector_point.z);       
    }
    else{
        float t_pos = STEP_SIZE;
        // Compute_Next_Increment(t_pos, camera_position_array, frame_position_array[frame_array_index]);
        Compute_Next_Increment(t_pos, camera_position_array,frame_position_array[frame_array_index-1], 
          frame_position_array[frame_array_index]);
    }

    float angle_vector_distance = Vector_Distance(camera_angle_array, frame_angle_array[frame_array_index]);
    printf ("\nLook at distance: %f\n",angle_vector_distance);
    if (angle_vector_distance < SNAP_ANGLE_DISTANCE){
      //snap onto look at position
      camera_angle_array[0] = frame_angle_array[frame_array_index].x;
      camera_angle_array[1] = frame_angle_array[frame_array_index].y;
      camera_angle_array[2] = frame_angle_array[frame_array_index].z;
      printf("\nSnapped In Look At\n");
      // printf("\nAngle Changes: %f %f %f\n", camera_angle_changes_array[0],camera_angle_changes_array[1],
      //   camera_angle_changes_array[2]);
      // move_position = 1;
    }
    else{
      float t_angle = TURN_SMOOTHNESS*STEP_SIZE;//(STEP_SIZE/angle_vector_distance);
      Compute_Next_Increment(t_angle, camera_angle_array, frame_angle_array[frame_array_index-1], 
        frame_angle_array[frame_array_index]);
    }

  printf("\n rendering done:\n");

}

  return next_frame_flag;
}
