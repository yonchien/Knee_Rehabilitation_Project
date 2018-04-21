/*
 *	Class: horizontal_alignment.c
 *	-----------------------------
 *	This module horizontally aligns the shank WIMU's so that the 
 *	the shank and thigh knee-joint angles are in a common reference frame
*/

#include "horizontal_alignment.h"


/* 
 *	Function: horzontal
 *	-------------------
 *	Performs a horizontal rotation of the shank knee-joint 
 *	angle data using the seel method
 *	
 *	thigh: The thigh WIMU's acc angles
 *	shank: The shank WIMU's knee-joint angles
 *	euler: The shank knee-joint angles to be rotated
 *	Note: The number of random samples used in the algorithm 
 *	      may be changed to accommodate different sensor sample rates
*/
void horizontal(vector *thigh, vector *shank, vector *euler, double *theta, int leg) {

	srand(time(NULL));                               // Seed the random number generator
	/* Initialization of member variables */
	int r = 0;
	double x;
	double phi_1 = 0;
	double phi_2 = 0;

	double e[SAMPLE_N][NMAX] = { 0 };                                    // Error Vectors
	double *j_vec = (double*)malloc(SAMPLE_N * sizeof(double));          // Jacobian
	double *result = (double*)malloc(SAMPLE_N * sizeof(double));
	double **V = (double**)malloc(1 * sizeof(double*));
	V[0] = (double*)malloc(1 * sizeof(double));
	double *W = (double*)malloc(1 * sizeof(double));

	int i = 0;
	int j = 0;

	for (i = 0; i < NMAX; i++) {

		for (j = 0; j < SAMPLE_N; j++) {                         // 1000 iterations 

			/* Chose random sample row based on thigh/shank matrix with fewest rows */
			if (thigh->total < shank->total)
				r = rand() % (thigh->total - 1) + 1;
			else
				r = rand() % (shank->total - 1) + 1;
			double *thigh_row = VECTOR_GET(*thigh, double *, r);
			double *shank_row = VECTOR_GET(*shank, double *, r);
			double g1[3] = { thigh_row[1], thigh_row[2], thigh_row[3] };        // Thigh angles
			double g2[3] = { shank_row[1], shank_row[2], shank_row[3] };        // Shank angles
			x = phi_2;

			double j1[3][1] = {                       // Thigh sensor coordinate system transposed
				{ cos(phi_1)*sin(*theta) },
				{ sin(phi_1)*sin(*theta) },
				{ cos(*theta) }
			};

			double j2[3][1] = {                       // Shank sensor coordinate system transposed
				{ cos(phi_2)*sin(*theta) },
				{ sin(phi_2)*sin(*theta) },
				{ cos(*theta) }
			};

			/* Compute the cross products */
			double cross_1[3] = { 0 };
			double cross_2[3] = { 0 };

			cross_1[0] = (j1[1][0] * g1[2] - j1[2][0] * g1[1]);
			cross_1[1] = (j1[2][0] * g1[0] - j1[0][0] * g1[2]);
			cross_1[2] = (j1[0][0] * g1[1] - j1[1][0] * g1[0]);

			cross_2[0] = (j2[1][0] * g2[2] - j2[2][0] * g2[1]);
			cross_2[1] = (j2[2][0] * g2[0] - j2[0][0] * g2[2]);
			cross_2[2] = (j2[0][0] * g2[1] - j2[1][0] * g2[0]);

			e[j][i] = norm(cross_1) - norm(cross_2);                // Save the error vector

			/* Compute the Jacobian */
			double j_phi = -((j2[1][0] * g2[2] - j2[2][0] * g2[1])*(g2[2] * sin(*theta)*cos(phi_2)) +
				(j2[2][0] * g2[0] - j2[0][0] * g2[2])*(g2[2] * sin(*theta)*sin(phi_2)) +
				(j2[0][0] * g2[1] - j2[1][0] * g2[0])*(-g2[1] * sin(*theta)*sin(phi_2) - g2[0] * sin(*theta)*cos(phi_2))) /
				norm(cross_2);
			 
			j_vec[j] = j_phi;                                      // Save Jacobian
		}

		/* Compute the pseudoinverse */
		int col_n = 1;
		double *PINV = (double *)malloc(LDA  * N * sizeof(double));       // Storage for pseudoinverse
		singular_value_decomposition(j_vec, PINV);

		double sum = 0;
		for (j = 0; j < SAMPLE_N; j++) {
			sum = sum + (PINV[j] * e[j][i]);
		}
		x = x - sum;
		phi_2 = x;                           // Update phi_2 angle
		free(PINV);                          // Free dynamically allocated storage
	}

	/* Final angle computation based on leg */
	if (leg == RIGHT)
		phi_2 = (PI * -1) + phi_2;
	else
		phi_2 = PI + phi_2;

	*theta = phi_2;                          // The final horizontal rotation angle

	horizontal_rotation(*theta, euler);      // Apply rotation to shank knee-joint angles

	/* Free dynamically allocated storage */
	free(j_vec);
	free(result);
	free(V[0]);
	free(V);
	free(W);

	return;
}



/*
 *	Function: singular_value_decomposition
 *	--------------------------------------
 *	Computes the pseudoinverse of a matrix using the
 *	singular value decomposition (SVD)
 *	
 *	a: The matrix (vector in this application) to be 
 *	   inverted
 *	PINV: Storage for the pseudoinverse
 */
void singular_value_decomposition(double *a, double *PINV){
	/* Locals */
	int m = M, n = N, lda = LDA, ldu = LDU, ldvt = LDVT, info, lwork;
	double wkopt;
	double* work;
	/* Local arrays */
	double s[N], vt[LDVT*N];
	double *u = (double*)malloc(LDU*M * sizeof(double));
	
	/* Query and allocate the optimal workspace */
	lwork = -1;
	dgesvd_("All", "All", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, &wkopt, &lwork,
		&info);
	lwork = (int)wkopt;
	work = (double*)malloc(lwork * sizeof(double));
	/* Compute SVD */
	dgesvd_("All", "All", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work, &lwork,
		&info);
	/* Check for convergence */
	if (info > 0) {
		printf("The algorithm computing SVD failed to converge.\n");
		exit(1);
	}

	/* Compute the pseudo inverse */
	double tempS;
	int i = 0;
	for (i = 0; i < K; i++) {
		if (s[i] < 1.0e-15) {
			tempS = 0.0;
		}
		else
			tempS = (double )(1 / s[i]);
		int m = M;
		int IONE = 1;
		dscal_(&m, &tempS, &(u[i*LDU]), &IONE);
	}
	double zpone = 1.0;
	double zzero = 0.0;
	int k = K;
	char CONJTRANS = 'C';
	dgemm_(&CONJTRANS, &CONJTRANS, &n, &m, &k, &zpone, vt, &ldvt, u, &m, &zzero, PINV, &n);

	free((void*)work);
	free((void*)u);
}

/*
 *	Function: horizontal_rotation
 *	-----------------------------
 *	Rotates the shanks knee-joint angles
 *	horizontal plane (XY) using a rotation matrix 
 *
 *	theta: Angle of horizontal rotation in radians
 *	euler: A vector holding the knee-joint angles to be rotated
*/
void horizontal_rotation(double theta, vector *euler) {

	/* Rotation matrix */
	double A[3][3] = {
		{ cos(theta), -sin(theta), 0 },
		{ sin(theta), cos(theta), 0 },
		{ 0,0,1 }
	};
	int i = 0;
	int j = 0;
	for (i = 0; i < euler->total; i++) {                          // Iterate through knee-joint angles
		double *euler_row = VECTOR_GET(*euler, double *, i);
		double temp_row[3] = {euler_row[0], euler_row[1], euler_row[2]};
		double new_row[3] = {0};

		/* Multiply angles by the rotation matrix A */
		int c = 0;
		int d = 0;
		int k = 0;
		double sum = 0;
		for (d = 0; d < 3; d++) {
			for (k = 0; k < 3; k++) {
				sum = sum + temp_row[k] * A[k][d];
			}
			new_row[d] = sum;
			sum = 0;
		}
		for (j = 0; j < 3; j++)
			euler_row[j] = new_row[j];                            // Update knee-joint angles
	}
	return;
}

void horizontal_rotation_gyr(vector *, double);
void horizontal_rotation_gyr(vector *gyr, double theta) {

	/* Rotation matrix */
	double A[3][3] = {
		{ cos(theta), -sin(theta), 0 },
		{ sin(theta), cos(theta), 0 },
		{ 0,0,1 }
	};
	int i = 0;
	int j = 0;
	for (i = 0; i < gyr->total; i++) {                          // Iterate through gyr data
		double *gyr_row = VECTOR_GET(*gyr, double *, i);
		double temp_row[3] = { gyr_row[1], gyr_row[2], gyr_row[3] };
		double new_row[3] = { 0 };

		/* Multiply angles by the rotation matrix A */
		int c = 0;
		int d = 0;
		int k = 0;
		double sum = 0;
		for (d = 0; d < 3; d++) {
			for (k = 0; k < 3; k++) {
				sum = sum + temp_row[k] * A[k][d];
			}
			new_row[d] = sum;
			sum = 0;
		}
		for (j = 0; j < 3; j++)
			gyr_row[j + 1] = new_row[j];                            // Update gyr data
	}
	return;
}

/* 
 *	Function: norm
 *	--------------
 *	Computes the norm of a length 3 vector 
 *	
 *	v: Vector containing 3 elements
 *	Returns: Norm of the vector
 */
double norm(double *v) {

	double norm = 0;
	int i = 0;
	for (i = 0; i < 3; i++) {
		norm = norm + pow(v[i], 2);
	}
	return sqrt(norm);
}


