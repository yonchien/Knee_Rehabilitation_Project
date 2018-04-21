/*
*	Quaternions_module.c
*	Provides implementation for Quaternions_module.h header file
*/

#include "quaternion.h"


int quaternion_calc(double *acc, double *gyr, double dt, int sample, double* euler, double *qosserv_prev, double *qfilt_prev, double *accf) {

	int i = 0;
	int j = 0;
	double dtt = dt;

	/*	Fill in the accF row with last data points in acc
	Note: this will eventually have to go through filtering */
	accf[0] = acc[30];
	accf[1] = acc[31];
	accf[2] = acc[32]; //Fill in accf row

					   /*	Only use the gradient decent portion for the first 110 samples
					   Note: this might need to be updated because safesens uses different sample frequency  */
	if (sample <= ACCF_LENGTH + 100) {

		/* Preform the gradient descent algorithm */
		double dqnorm = 1;
		double mu = 2 * dqnorm * dtt;
		double qosserv_one[4] = { 0 };
		gradient_descent(accf, qosserv_prev, mu, qosserv_one); //Finds gradient and normalizes result

															   /* Set the previous rows to the current one and end the outer for loop */
		for (j = 0; j < 4; j++) {
			qfilt_prev[j] = qosserv_one[j];
			qosserv_prev[j] = qosserv_one[j]; //Update qosserv_prev
		}

		/* Compute the euler angles */
		double angles[3];
		compute_euler_angles(qosserv_one, angles);
		euler[0] = angles[0];
		euler[1] = angles[1];
		euler[2] = angles[2];
		return 0;
	}
	/* Run the complete fusion algorithm on the rest of the samples */
	else {
		/* Perform the quaternion product algorithm */
		double dq[4] = { 0 };
		quaternion_product(qfilt_prev, gyr, dq);

		/* Calculate the qgyro filt row */
		double qgyro_filt[4] = { 0 };
		double dqnorm = 0;
		double qgyro_filt_norm = 0;
		for (j = 0; j < 4; j++) {
			dqnorm = dqnorm + pow(dq[j], 2);
			qgyro_filt[j] = qfilt_prev[j] + dq[j] * dtt;
			qgyro_filt_norm = qgyro_filt_norm + pow(qgyro_filt[j], 2);
		}

		/* Find the norm of the dq and qgyro filt rows */
		dqnorm = sqrt(dqnorm);
		double mu = 10 * dqnorm * dtt;
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

		/* Compute the euler angles */
		double angles_2[3];
		compute_euler_angles(qfilt, angles_2);
		euler[0] = angles_2[0];
		euler[1] = angles_2[1];
		euler[2] = angles_2[2];

		/* Update the qfilt prev and qosserv prev with the current rows */
		for (j = 0; j < 4; j++) {
			qfilt_prev[j] = qfilt[j]; //Update qfilt_prev
			qosserv_prev[j] = qosserv[j]; //Update qosserv_prev
		}
	}
	return 0;
}

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

int compute_euler_angles(double *euler, double *e) {

	double q1 = euler[0];
	double q2 = euler[1];
	double q3 = euler[2];
	double q4 = euler[3];

	e[0] = atan2(2 * (q1*q2 + q3*q4), 1 - 2 * (q2*q2 + q3*q3)) * 180 / PI;
	e[1] = asin(2 * (q1*q3 - q2*q4))*(180 / PI);
	e[2] = atan2(2 * (q1*q4 + q2*q3), 1 - 2 * (q3*q3 + q4*q4))*(180 / PI);

	return 0;
}

void gradient_descent(double *acc, double *q, double mu, double *gradient_result) {


	double q1 = q[0];
	double q2 = q[1];
	double q3 = q[2];
	double q4 = q[3];
	int i = 0;

	if (mu == 0.0) {
		mu = 0.0001;
	}

	double norm_norm = 0; //Used to normalize the result

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
		double norm = 0; //norm of the resultant matrix

		for (f = 0; f<4; f++) { //row of first matrix
			double sum = 0;
			for (k = 0; k<3; k++) {
				sum = sum + jg[f][k] * fg[k][0];
			}
			df[f][0] = sum;
			norm = norm + pow(sum, 2);
		}
		//     	if(norm == 0)
		//     		norm = 1;
		norm = sqrt(norm);

		double result_norm = 0; //Norm of this iterations result
		for (f = 0; f < 4; f++) {
			gradient_result[f] = q[f] - (mu*(df[f][0] / norm));
			result_norm = result_norm + pow(gradient_result[f], 2);
		}

		result_norm = sqrt(result_norm);
		//      	if (result_norm == 0)
		//      		result_norm = 1;

		for (f = 0; f < 4; f++) {
			gradient_result[f] = gradient_result[f] / result_norm;

			if (i == 9) {
				norm_norm = norm_norm + pow(gradient_result[f], 2); // compute final norm
			}

			q[f] = gradient_result[f];

		}
		q1 = gradient_result[0];
		q2 = gradient_result[1];
		q3 = gradient_result[2];
		q4 = gradient_result[3];

	}
	norm_norm = sqrt(norm_norm);
	//	if (norm_norm == 0)
	//		norm_norm = 1;
	for (i = 0; i < 4; i++) {
		gradient_result[i] = gradient_result[i] / norm_norm; //Normalize the final result
	}
}

