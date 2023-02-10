#include <stdio.h>
// Table of arctan's for use with CORDIC algorithm
// Store in decimal representation N = ((2^16)*angle_deg) / 180
#define ATAN_TAB_N 16
int atantable[ATAN_TAB_N] = {   0x4000,   //atan(2^0) = 45 degrees
                                0x25C8,   //atan(2^-1) = 26.5651
                                0x13F6,   //atan(2^-2) = 14.0362
                                0x0A22,   //7.12502
                                0x0516,   //3.57633
                                0x028B,   //1.78981
                                0x0145,   //0.895174
                                0x00A2,   //0.447614
                                0x0051,   //0.223808
                                0x0029,   //0.111904
                                0x0014,   //0.05595
                                0x000A,   //0.0279765
                                0x0005,   //0.0139882
                                0x0003,   //0.0069941
                                0x0002,   //0.0035013
                                0x0001    //0.0017485
};
// Inputs: 
//  theta = any (integer) angle in degrees
//  iterations = number of iterations for CORDIC algorithm, up to 16
int main(){
  volatile  int *iteration_pointer = ( int *) 0x80000000,
                *shift_pointer = ( int *) 0x80000004,
                *quad_pointer = ( int *) 0x80000008,
                *x1_pointer = ( int *) 0x8000000C,
                *theta_pointer = ( int *) 0x80000010,
                *start_pointer = ( int *) 0x80000014,

                *sin_pointer = ( int *) 0x80000018,
                *cos_pointer = ( int *) 0x8000001C,
                *done_register = ( int *) 0x80000020;

  int quadAdj, shift;
  int theta;
  int iterations;
  int sin_result, cos_result;


  theta = 75;
  iterations = 12;

  //Limit iterations to number of atan values in our table
  iterations = (iterations > ATAN_TAB_N) ? ATAN_TAB_N : iterations;
  //Shift angle to be in range -180 to 180
  while(theta < -180) theta += 360;
  while(theta > 180) theta -= 360;
  
  //convert angle to decimal representation N = ((2^16)*angle_deg) / 180
  if (theta < -90) {
        theta += 180;
        quadAdj = -1;
    } else if (theta > 90) {
        theta -= 180;
        quadAdj = -1;
    } else {
        quadAdj = 1;
    }

    if (theta < -45) {
        theta += 90;
        shift = -1;
    } else if (theta > 45) {
        theta -= 90;
        shift = 1;
    } else {
        shift = 0;
    }


    if (theta < 0) {
        theta = -theta;
        theta = ((unsigned int) theta << 10) / 45;
        theta = (unsigned int) theta << 4;
        theta = -theta;
    } else {
        theta = ((unsigned int) theta << 10) / 45;
        theta = (unsigned int) theta << 4;
    }
  
  //Initial values
  *x1_pointer = 0x4DBA;
  *theta_pointer = theta;
  *iteration_pointer = iterations;
  *shift_pointer = shift;
  *quad_pointer = quadAdj;
  *start_pointer = 1;
  
  while(!(*done_register)); // busy waiting
  
  sin_result = *sin_pointer;
  cos_result = *cos_pointer;


  printf("sin_result:%d\tcos_result:%d\t",sin_result,cos_result);

  return 0;
}
