/*
 *	Class: quaternions.c
 *	---------------------------
 *	Performs a quaternion fusion algorithm on WIMU data to
 *	compute knee-joint angles. the algorithm combines quaternion based
 *	calculations from both gyroscope and accelerometer data. A gradient
 *	descent algorithm is used to compute the direction of the gyroscope
 *	measurement error as a quaternion derivative
*/

#include "quaternions.h"
#include "preprocessing.h"

/*
 *	Function: set_up_filters
 *	------------------------
 *	Filters acc data and populates the Accf vectors with the 
 *	vectors first 11 rows
 *
 *	acc_orig: The acc vector to be filtered and added to Accf
 *	Returns: The populated Accf array 
*/
void set_up_filters(vector *acc_orig, double *accf) {

	int index = 0;
	int i = 0;
	/* Iterate the first 11 vector rows and filter/normalize data */
	for (i = 0; i <= ACCF_LENGTH; i++) {
		if (i == 0)
			continue;                                              // The first row is not used
		double *acc_curr_row = VECTOR_GET(*acc_orig, double *, i);
		double acc_norm = 0;
		int j = 0;
		for (j = 0; j < 3; j++) {
			acc_norm = (acc_norm + pow(acc_curr_row[j + 1], 2));
			accf[index] = (acc_curr_row[j + 1]);                // Add row to Accf
			index++;
		}
		acc_norm = sqrt(acc_norm);
		index = index - 3;
		for (j = 0; j < 3; j++) {
			accf[index] = accf[index] / acc_norm;              //Normalize the row data
			index++;
		}
	}
}


/* 
 *	Function: quaternion_calc 
 *	-------------------------
 *	Performs the quaternion fusion algorithm on acc and gyr data
 *	to find knee-joint angles angles
 *
 *	acc_orig: The acc data from a WIMU
 *	gyr_orig: The gyr data from a WIMU
 *	euler_angles: Vector to be populated with knee-joint angles
 *	acc: Array that contains the Accf filtered data
 */
void quaternion_calc(vector *acc_orig, vector *gyr_orig, vector *euler_angles, double *acc) {

	/* Initilaize helper variables */
	double qosserv_prev[4] = { 0 };
	double qfilt_prev[4] = { 0 };
	double accf[3] = { 0 };
	qosserv_prev[0] = 1;
	double time_stamp = 0;

	int i = 0;
	/* 	Iterate through the length of the acc matrix to perform quaternion calculations */
	for (i = ACCF_LENGTH + 1; i < acc_orig->total; i++) {

		/* 	Shift the elements of the acc vector to make space for new data
		The acc vector only holds 11 rows of data at any time */
		int j = 0;
		for (j = 0; j < ACC_LENGTH - 3; j++) {
			acc[j] = acc[j + 3]; //Move all acc elements down three
		}
		/*	Get the current and previous rows of the original acc matrix */
		double *acc_curr_row = VECTOR_GET(*acc_orig, double *, i);
		double *acc_pre_row = VECTOR_GET(*acc_orig, double *, i - 1);
		double dt = (acc_curr_row[0]) - (acc_pre_row[0]);
		double acc_norm = 0;
		/*	Fill in the acc vector with the new data(row) from the current
		original acc row and Normalize*/
		for (j = 0; j < 3; j++) {
			acc_norm = acc_norm + pow(acc_curr_row[j + 1], 2);
			acc[ACC_LENGTH - (3 - j)] = acc_curr_row[j + 1]; //Fill in new row of acc data
		}

		acc_norm = sqrt(acc_norm);
		for (j = 0; j < 3; j++) {
			acc[ACC_LENGTH - (3 - j)] = acc[ACC_LENGTH - (3 - j)] / acc_norm; //Normalize the acc data
		}
		/* 	Skip the first line in the calculations to follow matlab code */
		if (i == 11) {
			continue;
		}
		time_stamp = acc_curr_row[0];
		/*	Fill in the accf row with filtered data points from acc */
		my_filter(acc, accf);

	   /*	Only use the gradient decent portion for the first 110 samples
	   Note: this might need to be updated in the future */
		if (i <= ACCF_LENGTH + 100) {
			/* Preform the gradient descent algorithm */
			double dqnorm = 1;
			double mu = 2 * dqnorm * dt;
			double qosserv_one[4] = { 0 };
			gradient_descent(accf, qosserv_prev, mu, qosserv_one); // Finds gradient and normalizes result
			/* Set the previous rows to the current one and end the outer for loop */
			for (j = 0; j < 4; j++) {
				qfilt_prev[j] = qosserv_one[j];
				qosserv_prev[j] = qosserv_one[j]; //Update qosserv_prev
			}
			/* Compute the eulerian angles */
			double angles[4] = { 0 };
			compute_euler_angles(qfilt_prev, angles);
			angles[3] = time_stamp;
			/*	Add these angles to the permenant matrix */
			double *angles_per = (double*)calloc(4, sizeof(double));
			memcpy(angles_per, angles, 4 * sizeof(double));
			VECTOR_ADD(*euler_angles, angles_per);
		}

		/* Run the complete fusion algorithm on the rest of the samples */
		else {
			/* Get the current row of the gyr original matrix */
			double *gyr = VECTOR_GET(*gyr_orig, double*, i);
			double qgyr[4] = { 0 };
			for (j = 0; j < 3; j++) {
				qgyr[j + 1] = gyr[j + 1]; //Create the gyr quaternion
			}
			/* Preform the quaternion product algorithm */
			double dq[4] = { 0 };
			quaternion_product(qfilt_prev, qgyr, dq);
			/* Calculate the qgyro filt row */
			double qgyro_filt[4] = { 0 };
			double dqnorm = 0;
			double qgyro_filt_norm = 0;
			for (j = 0; j < 4; j++) {
				dqnorm = dqnorm + pow(dq[j], 2);
				qgyro_filt[j] = qfilt_prev[j] + dq[j] * dt;
				qgyro_filt_norm = qgyro_filt_norm + pow(qgyro_filt[j], 2);
			}
			/* Find the norm of the dq and qgyro filt rows */
			dqnorm = sqrt(dqnorm);
			double mu = 10 * dqnorm * dt;
			qgyro_filt_norm = sqrt(qgyro_filt_norm);
			for (j = 0; j < 4; j++) {
				qgyro_filt[j] = qgyro_filt[j] / qgyro_filt_norm; //Normalize the qgyro_filt
			}
			/* Preform the gradient descent algorithm */
			double qosserv[4] = { 0 };
			gradient_descent(accf, qosserv_prev, mu, qosserv); //Finds the gradient and normalizes the result
			/* Finish the fusion algorithm for this row and update qfilt */
			double qfilt[4] = { 0 };
			double qfilt_norm = 0;
			for (j = 0; j < 4; j++) {
				qfilt[j] = qgyro_filt[j] * ALFA + qosserv[j] * (1 - ALFA);
				qfilt_norm = qfilt_norm + pow(qfilt[j], 2);
			}
			qfilt_norm = sqrt(qfilt_norm);
			for (j = 0; j < 4; j++) {
				qfilt[j] = qfilt[j] / qfilt_norm; //Normalize the qfilt 
			}
			/* Compute the eulerian angles */
			double angles_2[4] = { 0 };
			compute_euler_angles(qfilt, angles_2);
			angles_2[3] = time_stamp;
			/* Add these angles to the permenant matrix */
			double *angles_per = (double*)calloc(4, sizeof(double));
			memcpy(angles_per, angles_2, 4 * sizeof(double));
			VECTOR_ADD(*euler_angles, angles_per);
			/* Update the qfilt prev and qosserv prev with the current rows */
			for (j = 0; j < 4; j++) {
				qfilt_prev[j] = qfilt[j]; //Update qfilt_prev
				qosserv_prev[j] = qosserv[j]; //Update qosserv_prev
			}
		}
	}
	return;
}

/*
 *	Function: my_filter
 *	-------------------
 *	Perform a low pass filter on the acc data, 
 *	returning the last X Y Z values
 *
 *	acc: The acc data to be filtered
 *	Accf: Array that will be populated with the
 *	      Accf filtered data
*/
void my_filter(double *acc, double *accf) {

	double acc_tmp[10];
	double norm = 0;
	int count = 0;
	int i = 0;
	/* Filter X data points */
	for (i = 0; i <= 27; i += 3){
		acc_tmp[count] = acc[i];
		count++;
	}
	filter_data(acc_tmp, FILTER_ORDER, MFCUTOFF, MFSIZE);
	accf[0] = acc_tmp[9];
	norm = norm + pow(acc_tmp[9], 2);
	count = 0;
	/* Filter Y data points */
	for (i = 1; i <= 28; i += 3) {
		acc_tmp[count] = acc[i];
		count++;
	}
	filter_data(acc_tmp, FILTER_ORDER, MFCUTOFF, MFSIZE);
	accf[1] = acc_tmp[9];
	norm = norm + pow(acc_tmp[9], 2);
	count = 0;
	/* Filter Z data points */
	for (i = 2; i <= 29; i += 3) {
		acc_tmp[count] = acc[i];
		count++;
	}
	filter_data(acc_tmp, FILTER_ORDER, MFCUTOFF, MFSIZE);
	accf[2] = acc_tmp[9];
	norm = norm + pow(acc_tmp[9], 2);
	norm = sqrt(norm);
	for (i = 0; i < 3; i++) {
		accf[i] = accf[i] / norm;
	}
}

/* 
 *	Function: quaternion_product
 *	----------------------------
 *	Calculates the product of two quaternions
 *
 *	a: The first quaternion
 *	b: the second quaternion
 *	Returns: The product of a and b
 */
void quaternion_product(double *a, double *b, double *p) {

	double half = 0.5;

	double a1 = a[0];
	double b1 = b[0];
	double a2 = a[1];
	double b2 = b[1];
	double a3 = a[2];
	double b3 = b[2];
	double a4 = a[3];
	double b4 = b[3];

	p[0] = half*(a1*b1 - a2*b2 - a3*b3 - a4*b4);
	p[1] = half*(a1*b2 - a2*b1 + a3*b4 - a4*b3);
	p[2] = half*(a1*b3 - a2*b4 + a3*b1 + a4*b2);
	p[3] = half*(a1*b4 + a2*b3 - a3*b2 + a4*b1);
}

/* 
 *	Function: compute_euler_angles
 *	------------------------------
 *	computes the eulerian angles given a quaternion
 *
 *	q: The quaternion to convert
 *	Returns: Array populated with x,y,z eularian angles
 */
void compute_euler_angles(double *q, double *result) {

	double q1 = q[0];
	double q2 = q[1];
	double q3 = q[2];
	double q4 = q[3];

	result[0] = atan2(2 * (q1*q2 + q3*q4), 1 - 2 * (q2*q2 + q3*q3)) * 180 / PI;
	result[1] = asin(2 * (q1*q3 - q2*q4))*(180 / PI);
	result[2] = atan2(2 * (q1*q4 + q2*q3), 1 - 2 * (q3*q3 + q4*q4))*(180 / PI);
}

/* 
 *	Function: gradient_descent
 *	--------------------------
 *	Performs the gradient descent portion of the quaternion 
 *	fusion algorithm
 *	
 *	acc: Array populated with the Accf filterd data 
 *	q: Array with the qosserv_prev data
 *	mu: The calculated mu value to be used
 *	Returns: The newly calculated qosserv array
 */
void gradient_descent(double *acc, double *q, double mu, double *gradient_result) {

	int i = 0;
	double q1 = q[0];
	double q2 = q[1];
	double q3 = q[2];
	double q4 = q[3];
	if (mu == 0) {
		mu = 0.0001;
	}
	double norm_norm = 0;               //Used to normalize the result 
	for (i = 0; i < 10; i++) {
		double fg1 = 2 * (q2*q4 - q1*q3) - acc[0];
		double fg2 = 2 * (q1*q2 + q3*q4) - acc[1];
		double fg3 = 2 * (0.5 - pow(q2, 2) - pow(q3, 2)) - acc[2];
		double fg[3][1] = { fg1,fg2,fg3 };
		double jg[4][3] = {
			{ -2 * q3,2 * q2,0 },
			{ 2 * q4,2 * q1,-4 * q2 },
			{ -2 * q1,2 * q4,-4 * q3 },
			{ 2 * q2,2 * q3,0 }
		};
		double df[4][1] = { 0 };
		int f = 0;
		int j = 0;
		int k = 0;
		double norm = 0;               //norm of the resultant matrix

		/* Multiply the jg and fg matrices */
		for (f = 0; f<4; f++) {        //row of first matrix
			double sum = 0;
			for (k = 0; k<3; k++) {
				sum = sum + jg[f][k] * fg[k][0];
			}
			df[f][0] = sum;
			norm = norm + pow(sum, 2);
		}
		norm = sqrt(norm);
		double result_norm = 0;       //Norm of this iterations result

		for (f = 0; f < 4; f++) {
			gradient_result[f] = q[f] - (mu*(df[f][0] / norm));
			result_norm = result_norm + pow(gradient_result[f], 2);
		}
		result_norm = sqrt(result_norm);

		for (f = 0; f < 4; f++) {
			gradient_result[f] = gradient_result[f] / result_norm;
			if (i == 9) {
				norm_norm = norm_norm + pow(gradient_result[f], 2);    // compute final norm 
			}
			q[f] = gradient_result[f];
		}
		q1 = gradient_result[0];
		q2 = gradient_result[1];
		q3 = gradient_result[2];
		q4 = gradient_result[3];

	}
	norm_norm = sqrt(norm_norm);
	for (i = 0; i < 4; i++) {
		gradient_result[i] = gradient_result[i] / norm_norm;           //Normalize the final result 
	}
}

