/* 
 *	Class: resampling_module.h
 *	--------------------------
*/

#ifndef RESAMPLING_H
#define RESAMPLING_H

#include <limits.h>
#include "vector.h"

#define EUL_1 0
#define EUL_2 1

typedef struct vectors {

	vector *eul_1;
	vector *eul_1_tmp;
	vector *eul_2;
	vector *eul_2_tmp;
	vector *eul_3;
	vector *eul_3_tmp;
	vector *eul_4;
	vector *eul_4_tmp;
	int min_right;
	int min_left;
	int min_both;

}vec;


int resampling_right(vec *);

int resampling_left(vec *);

int resampling_both(vec *);

void set_up_resample(vector *, vector *);

#endif