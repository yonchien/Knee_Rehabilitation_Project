/*
*	Class: preprocessing.h
*	----------------------
*/

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "liir.h"

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#define STOPPER -32768                 // Smaller than any Filter Input
#define MEDIAN_FILTER_SIZE 20          // Filter Size for Median Filter
#define FILTER_ORDER 2                 // Filter Order
#define CUTOFF 0.05                    // Cutoff Frequency For Filter
#define MFCUTOFF 0.005                 // Cutoff Frequency For Quaternion Filter
#define MFSIZE 10                      // Size of My Filter
#define SCALE_ON 1                     // Filter Scaling On
#define SCALE_OFF 0                    // Filter Scaling Off

/* Used for median filter */
struct pair
{
	struct pair *point;                              /* Pointers forming list linked in sorted order */
	double  value;                                   /* Values to sort */
};

/* Used for median filter */
typedef struct filter {
	struct pair buffer[MEDIAN_FILTER_SIZE];          /* Buffer of nwidth pairs */
	struct pair *datpoint;                           /* Pointer into circular buffer of data */
	struct pair small;                               /* Chain stopper */
	struct pair big;                                 /* Pointer to head (largest) of linked list.*/
}filter;

int convert_deg_rad(vector *);

int lininterp1(vector *);

int lininterp2(vector *);

void d_coeff(int, double, double*);

void c_coeff(int, double, int, double*);

void filter_vector(vector *, int, double);

void filter_data(double *, int, double, int);

void filter_vector_helper(vector*, double*, double*, int, int);

void filter_data_helper(double*, double*, double*, int, int, int);

double median_filter(double, filter*);

void median_filter_set_up(vector *);

#endif 