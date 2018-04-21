/* 
 *	Class: resampling.c
 *	-------------------
 *	This module temporally alignes the data from all of the WIMUs
 *	Note: this assumes that the timestamps are already aligned
 *	with an absolute starting timestamp, this is to be completed
 *	by sending a signal for all of the WIMUs to begin calculations
 *	over bluetooth/wifi
*/
#include "resampling.h"

/*
 *	Function: resampling_right
 *	--------------------------
 *	Resamples the data from the right leg
 *
 *	v: Vec struct containing the right leg data
 *	Returns: 0 if the function is completed sucessfully,
 *           or 1 if there was an error
 *	Note: this function assumes that the timestamps are sorted in acending order (Which they should be)
*/
int resampling_right(vec *v) {

	int i = 0;
	int j = 0;

	/* Use the smallest matrix */
	if (v->min_right == EUL_1) {

		vector *tmp = v->eul_2_tmp;
		for (i = 0; i < v->eul_1->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_1, double*, i);       // Current row of smaller matrix
			for (j = 0; j < v->eul_2->total; j++) {
				double *temp = VECTOR_GET(*tmp, double*, j);        // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0) {
					temp_diff = (-1)*temp_diff;                     // abs val of the difference 
				}
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                           // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row = VECTOR_GET(*v->eul_2, double *, i);
				double *min_row = VECTOR_GET(*tmp, double *, indx);
				old_row[0] = min_row[0];
				old_row[1] = min_row[1];
				old_row[2] = min_row[2];
				old_row[3] = min_row[3];
			}
		}
		VECTOR_DELETE_SEC(*v->eul_2, v->eul_1->total, v->eul_2->total);// Delete the extra data on larger vector
	}
	else if (v->min_right == EUL_2) {

		vector *tmp = v->eul_1_tmp;
		for (i = 0; i < v->eul_2->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_2, double*, i);         // Current row of smaller matrix
			for (j = 0; j < v->eul_1->total; j++) {
				double *temp = VECTOR_GET(*tmp, double*, j);          // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0)
					temp_diff = (-1)*temp_diff;                       // abs val of the difference 
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                             // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row = VECTOR_GET(*v->eul_1, double *, i);
				double *min_row = VECTOR_GET(*tmp, double *, indx);
				old_row[0] = min_row[0];
				old_row[1] = min_row[1];
				old_row[2] = min_row[2];
				old_row[3] = min_row[3];
			}
		}
		VECTOR_DELETE_SEC(*v->eul_1, v->eul_2->total, v->eul_1->total);// Delete the extra data on larger vector
	}
	else {
		printf("Incorrect parameters!\n");
		return 1;
	}

	return 0;
}

/*
 *	Function: resampling_left
 *	-------------------------
 *	Resamples the data from the left leg
 *
 *	v: Vec struct containing the right leg data
 *	Returns: 0 if the function is completed sucessfully,
 *           or 1 if there was an error
 *	Note: this function assumes that the timestamps are sorted in acending order (Which they should be)
*/
int resampling_left(vec *v) {

	int i = 0;
	int j = 0;

	/* Use the smallest matrix */
	if (v->min_left == EUL_1) {
		vector *tmp = v->eul_4_tmp;
		for (i = 0; i < v->eul_3->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_3, double*, i);          // Current row of smaller matrix
			for (j = 0; j < v->eul_4->total; j++) {
				double *temp = VECTOR_GET(*tmp, double*, j);           // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0) {
					temp_diff = (-1)*temp_diff;                        // abs val of the difference 
				}
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                              // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row = VECTOR_GET(*v->eul_4, double *, i);
				double *min_row = VECTOR_GET(*tmp, double *, indx);
				old_row[0] = min_row[0];
				old_row[1] = min_row[1];
				old_row[2] = min_row[2];
				old_row[3] = min_row[3];
			}
		}
		VECTOR_DELETE_SEC(*v->eul_4, v->eul_3->total, v->eul_4->total);// Delete the extra data on larger vector
	}
	else if (v->min_left == EUL_2) {
		vector *tmp = v->eul_3_tmp;
		for (i = 0; i < v->eul_4->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_4, double*, i);          // Current row of smaller matrix
			for (j = 0; j < v->eul_3->total; j++) {
				double *temp = VECTOR_GET(*tmp, double*, j);           // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0)
					temp_diff = (-1)*temp_diff;                        // abs val of the difference 
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                              // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row = VECTOR_GET(*v->eul_3, double *, i);
				double *min_row = VECTOR_GET(*tmp, double *, indx);
				old_row[0] = min_row[0];
				old_row[1] = min_row[1];
				old_row[2] = min_row[2];
				old_row[3] = min_row[3];
			}
		}
		VECTOR_DELETE_SEC(*v->eul_3, v->eul_4->total, v->eul_3->total);// Delete the extra data on larger vector
	}
	else
		return 1;
	return 0;
}

/*
*	Function: resampling_both
*	--------------------------
*	Resamples the data from both legs
*
*	v: Vec struct containing the right leg data
*	Returns: 0 if the function is completed sucessfully,
*           or 1 if there was an error
*	Note: This function should only be called after the 
*	right and left legs are resampled individually
*/
int resampling_both(vec *v) {

	int i = 0;
	int j = 0;

	/* Use the smallest matrix */
	if (v->min_both == EUL_1) {
		vector *tmp_3 = v->eul_3_tmp;
		vector *tmp_4 = v->eul_4_tmp;
		for (i = 0; i < v->eul_1->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_1, double*, i);            // Current row of smaller matrix
			for (j = 0; j < v->eul_4->total; j++) {
				double *temp = VECTOR_GET(*tmp_4, double*, j);           // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0) {
					temp_diff = (-1)*temp_diff;                          // abs val of the difference 
				}
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                                // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row_3 = VECTOR_GET(*v->eul_3, double *, i);
				double *old_row_4 = VECTOR_GET(*v->eul_4, double *, i);
				double *min_row_3 = VECTOR_GET(*tmp_3, double *, indx);
				double *min_row_4 = VECTOR_GET(*tmp_4, double *, indx);

				for (j = 0; j < 4; j++) {
					old_row_3[j] = min_row_3[j];
					old_row_4[j] = min_row_4[j];
				}
				j = 0;
			}
		}
		VECTOR_DELETE_SEC(*v->eul_3, v->eul_1->total, v->eul_3->total);// Delete the extra data on larger vector
		VECTOR_DELETE_SEC(*v->eul_4, v->eul_1->total, v->eul_4->total);
	}
	else if (v->min_both == EUL_2) {
		vector *tmp_1 = v->eul_1_tmp;
		vector *tmp_2 = v->eul_2_tmp;
		for (i = 0; i < v->eul_3->total; i++) {
			double temp_diff = 0;
			double min_diff = INT_MAX;
			int indx = 0;
			double *curr = VECTOR_GET(*v->eul_3, double*, i);          // Current row of smaller matrix
			for (j = 0; j < v->eul_1->total; j++) {
				double *temp = VECTOR_GET(*tmp_1, double*, j);         // Temp row of larger matrix
				temp_diff = temp[3] - curr[3];
				if (temp_diff < 0.0) {
					temp_diff = (-1)*temp_diff;                        // abs val of the difference 
				}
				if (temp_diff < min_diff) {
					min_diff = temp_diff;                              // set new min and index
					indx = j;
				}
			}
			if (i != indx) {
				double *old_row_1 = VECTOR_GET(*v->eul_1, double *, i);
				double *old_row_2 = VECTOR_GET(*v->eul_2, double *, i);
				double *min_row_1 = VECTOR_GET(*tmp_1, double *, indx);
				double *min_row_2 = VECTOR_GET(*tmp_2, double *, indx);

				for (j = 0; j < 4; j++) {
					old_row_1[j] = min_row_1[j];
					old_row_2[j] = min_row_2[j];
				}
				j = 0;
			}
		}
		VECTOR_DELETE_SEC(*v->eul_1, v->eul_3->total, v->eul_1->total);// Delete the extra data on larger vector
		VECTOR_DELETE_SEC(*v->eul_2, v->eul_3->total, v->eul_2->total);
	}
	else
		return 1;

	return 0;
}

/* 
 *	Function: set_up_resample
 *	-------------------------
 *	Sets up the resampling functions by populating a 
 *	temporary vector with the original vectors data
 *
 *	orig_vector: The original vector to copy
 *	new_vector: The new vector that will be populated 
 *	with the same data as orig_vector
*/
void set_up_resample(vector *orig_vec, vector *new_vec) {

	int i = 0;
	int j = 0;
	for (i = 0; i < orig_vec->total; i++) {
		double *orig_row = VECTOR_GET(*orig_vec, double*, i);
		double *new_row = malloc(4 * sizeof(double));
		for (j = 0; j < 4; j++)
			new_row[j] = orig_row[j];
		VECTOR_ADD(*new_vec, new_row);
	}
}