/*
 *	Class: txt2mat.h
 *	----------------
*/

#ifndef TXT2MAT_H
#define TXT2MAT_H

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

#define PI 3.14159265
#define EUL 0
#define NO_EUL 1

/* Used to hold the FILE and vector pointers for individual WIMU's for the OLD BOARDS */
typedef struct prev_files {
	FILE *file_acc;
	FILE *file_gyr;
	FILE *file_ts;
	vector *acc_vec;
	vector *gyr_vec;
	int num_lines;
	/* For testing purposes */
	int file_num;
	int record_num[100];
}Prev_Files;

/* Used to hold information about all WIMU's */
typedef struct WIMU_Files {
	int WIMU_rt;
	int WIMU_rs;
	int WIMU_lt;
	int WIMU_ls;
	int line_number_rt;
	int line_number_rs;
	int line_number_lt;
	int line_number_ls;
	vector *acc_rt;
	vector *gyr_rt;
	vector *acc_rs;
	vector *gyr_rs;
	vector *acc_lt;
	vector *gyr_lt;
	vector *acc_ls;
	vector *gyr_ls;
	vector *eul_rt;
	vector *eul_rs;
	vector *eul_lt;
	vector *eul_ls;
}WIMU_Files;

/* Used to hold the FILE pointers of the SENS and EULER files */
typedef struct sens_files {
	FILE *sens_rt;
	FILE *sens_rs;
	FILE *sens_lt;
	FILE *sens_ls;
	FILE *eul_rt;
	FILE *eul_rs;
	FILE *eul_lt;
	FILE *eul_ls;
	WIMU_Files *all_files;
	int record_number;
	int *sensor_nums_r;
	int *sensor_nums_l;
	int euler;
}Sens_Files;

void scan_prev_files(Prev_Files*, int);

void scan_sens_files(Sens_Files*);

int open_prev_files(Prev_Files*, int);

int open_sens_files(Sens_Files*);

int compare_double(double, double);

int cut_record(Prev_Files*, vector*, vector*, int);

void print_vectors(Prev_Files*, int);

int record_size(Prev_Files *, int);

#endif