/*
*	Class: quaternions.h
*	--------------------
*/

#ifndef QUATERNIONS_MODULE_H
#define QUATERNIONS_MODULE_H

#include "vector.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define ACCF_LENGTH 10
#define ALFA 0.999
#define PI 3.14159265
#define REAL_TIME 4
#define CURR_PREV 8
#define ACC_LENGTH 30


void set_up_filters(vector *, double *);

void quaternion_calc(vector *, vector *, vector *, double *);

void gradient_descent(double *, double *, double, double *);

void quaternion_product(double *, double *, double *);

void compute_euler_angles(double *, double *);

void my_filter(double *, double *);

#endif