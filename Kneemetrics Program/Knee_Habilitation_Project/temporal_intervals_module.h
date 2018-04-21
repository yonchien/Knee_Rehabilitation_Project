/*
*	Temporal_instants_module.h
*	This module will compute the stride length(SL) and clearance
*/

#ifndef TEMPORAL_INTERVALS_MODULE_H
#define TEMPORAL_INTERVALS_MODULE_H

#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PEAK_HEIGHT 2.0                       // The minimum peak height
#define MIN 10000                             // Arbitrary large value

typedef struct gait_parameters {
	vector *intervals_r;
	vector *intervals_l;
	vector *stridelength_r;
	vector *stridelength_l;
	vector *sp_r;
	vector *sp_l;
	double cadence_r;
	double cadence_l;
	double gate_v_r;
	double gate_v_l;
}gait;

void temporal_instants_1(vector *, vector *, int *, vector *, int);

int *get_peaks(vector *, int, int *);

void single_leg_temporal_intervals(vector *, vector *, vector *);

void combined_temporal_intervals(vector *, vector *, vector *, vector *);

void compute_stridelength(vector *, vector *, vector *, vector *, double, int);

double sample_freq(vector *);

void print_gait(gait *);

#endif	