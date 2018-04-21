/*
 *	Class: temporal_intervals_module.c
 *	----------------------------------
 *	Computes various gait parameters using the WIMU data and 
 *	knee-joint angles
*/

#include "temporal_intervals_module.h"

/*
 *	Function: temporal_instants_1
 *	-----------------------------
 *	Finds and stores the index and X axis gyro data for
 *	the Toe-Off, Heel_Strike, and Shank Vertical tempral 
 *	instants 
 *
 *	roll: The knee-joint angles contianting roll data 
 *	      in the first column of the vector 
 *	gyr: The gyro vector
 *	peaks: An ordered array containing the index of each peak
 *	instants: A vector to be updated with the temporal instants 
 *	          in the following order [T_O, T_O_I, H_S, H_S_I, S_V, S_V_I]
*/
void temporal_instants_1(vector *roll, vector *gyr, int *peaks, vector *instants, int num_peaks) {

	int peaks_length = num_peaks;
	int roll_length = roll->total;
	int i = 0;
	int prev_toe_index = 0;

	/* Iterate through all of the peaks */
	for (i = 0; i < peaks_length; i++) {
		int j = peaks[i];
		double *gyr_prev = VECTOR_GET(*gyr, double*, j - 1);
		double *gyr_curr = VECTOR_GET(*gyr, double*, j);

		while (gyr_prev[1] <= gyr_curr[1]) { // Find toe off index
			j--;
			gyr_prev = VECTOR_GET(*gyr, double*, j - 1);
			gyr_curr = VECTOR_GET(*gyr, double*, j);
		}

		if (j == prev_toe_index)                    // Continue if peak alligned with previous toe-off index
			continue;
		else
			prev_toe_index = j;

		double *instants_row = (double*)calloc(6, sizeof(double)); // Create a new row in the vector
		instants_row[0] = gyr_curr[1];              // Index of toe-off
		instants_row[1] = j;                        // Gyro data at toe-off
		j = peaks[i];
		int done = 0;
		int y = 0;
		for (y = j; y < gyr->total; y++) {         // Find heel strike starting from peak index
			double *heel_temp = VECTOR_GET(*gyr, double*, y);
			if (heel_temp[1] < 0) {
				instants_row[3] = y;               // Index of heel strike
				double *temp = VECTOR_GET(*gyr, double*, (int)instants_row[3]);
				instants_row[2] = temp[1];         // Gyro data at heel strike
				done = 1;
			}
			if (done == 1)
				break;
		}
		if (done == 0) {                           // Find min if data points dont go negative 
			double min_value = MIN;
			int min_index = 0;
			for (y = j; y < gyr->total; y++) {
				double *temp = VECTOR_GET(*gyr, double*, y);
				if (temp[1] < min_value) {
					min_value = temp[1];
					min_index = y;
				}
			}
			if (min_value != MIN) {
				instants_row[3] = min_index;           // Index of the heel strike
				double *temp = VECTOR_GET(*gyr, double*, (int)instants_row[3]);
				instants_row[2] = temp[1];             // Gyro data at heel strike
			}
		}
		done = 0;
		for (y = (int)instants_row[3]; y < roll_length; y++) {  // Find mid stance index 
			double *roll_temp = VECTOR_GET(*roll, double*, y);
			if (roll_temp[0] < 0) {
				instants_row[5] = y;               // Index of the mid stance
				double *temp = VECTOR_GET(*gyr, double *, (int)instants_row[5]);
				instants_row[4] = temp[1];         // Gyro data at the mid stance
				done = 1;
			}
			if (done == 1)
				break;
		}
		if (done == 0) {
			double min_value = MIN;
			int min_index = 0;
			for (y = (int)instants_row[3]; y < roll_length; y++) {
				double *roll_temp = VECTOR_GET(*roll, double *, y);
				if (roll_temp[0] < min_value) {
					min_value = roll_temp[0];
					min_index = y;
				}
			}
			if (min_value != MIN) {
				instants_row[5] = min_index;           // Index of the mid stance
				double *temp = VECTOR_GET(*gyr, double *, (int)instants_row[5]);
				instants_row[4] = temp[1];             // Gyro data at the mid stance
			}
		}
		VECTOR_ADD(*instants, instants_row);       // Add this row to the vector
	}

}

/*
 *	Fuction: single_leg_temporal_intervals
 *	--------------------------------------
 *	Computes the Gait Cycle Time (GCT), Swing Phase (SW_PH),
 *	and the Stance Phase (ST_PH)
 *
 *	intervals: A vector to be updated with the temporal intervals
 *	           in the following order: [GCT, SW_PH, ST_PH]
 *	instants: A vector containing the temporal instants 
 *	t: the gyro vector containing the time stamps in position [0]
*/
void single_leg_temporal_intervals(vector *intervals, vector *instants, vector *t) {
	double *starting_time = VECTOR_GET(*t, double*, 0);
	int i = 0;
	for (i = 1; i < instants->total; i++) {
		double *int_row = (double*)calloc(3, sizeof(double));            // Storage for the intervals row
		/* Vector row initialization */
		double *inst_curr = VECTOR_GET(*instants, double *, i);
		double *inst_prev = VECTOR_GET(*instants, double *, i - 1);
		double *toe_curr = VECTOR_GET(*t, double*, (int)inst_curr[1]);
		double *toe_prev = VECTOR_GET(*t, double*, (int)inst_prev[1]);
		double *heel_prev = VECTOR_GET(*t, double*, (int)inst_prev[3]);

		int_row[0] = (toe_curr[0] - starting_time[0]) - (toe_prev[0] - starting_time[0]);        // GCT
		int_row[1] = (heel_prev[0] - starting_time[0]) - (toe_prev[0] - starting_time[0]);       // SW_PH
		int_row[2] = (toe_curr[0] - starting_time[0]) - (heel_prev[0] - starting_time[0]);       // ST_PH

		VECTOR_ADD(*intervals, int_row);
	}
}

/*
 *	Function: combined_temporal_intervals
 *	-------------------------------------
 *	Computes the Initial Double Support (IDS), 
 *	Terminal Double Support (TDS), and Double Support 
 *	Interval (DSI)
 *	
 *	r_instants: A vector containing temporal instants 
 *             for the right leg
 *	l_instants: A vector containing temporal instants 
 *             for the left leg
 *	ds: A vector that will be populated with the double
 *      support interval data 
 *	t: A vector containing the time stamps in the [0] 
 *     position
*/
void combined_temporal_intervals(vector *r_instants, vector *l_instants, vector *ds, vector *t) {

	double *r_temp = VECTOR_GET(*r_instants, double*, 0);
	double *l_temp = VECTOR_GET(*l_instants, double*, 0);
	double *starting_time = VECTOR_GET(*t, double*, 0);

	int i = 0;
	for (i = 1; i < ds->total; i++) {
		/* initialization of necessary vector rows */
		double *l_instants_curr = VECTOR_GET(*l_instants, double*, i);
		double *l_toe_curr = VECTOR_GET(*t, double*, (int)l_instants_curr[1]);
		double *l_instants_prev = VECTOR_GET(*r_instants, double*, i - 1);
		double *l_toe_pre = VECTOR_GET(*t, double*, (int)l_instants_prev[1]);
		double *l_heel_pre = VECTOR_GET(*t, double*, (int)l_instants_prev[3]);
		double *r_instants_curr = VECTOR_GET(*r_instants, double*, i);
		double *r_toe_curr = VECTOR_GET(*t, double*, (int)r_instants_curr[1]);
		double *r_instants_prev = VECTOR_GET(*r_instants, double*, i - 1);
		double *r_toe_prev = VECTOR_GET(*t, double*, (int)r_instants_prev[1]);
		double *r_heel_prev = VECTOR_GET(*t, double*, (int)r_instants_prev[3]);
		double *ds_row = (double*)calloc(1, sizeof(double));

		if (r_temp[1] >= l_temp[1]) {             // Stride starts  with the right leg
			double IDS = (l_toe_pre[0] - starting_time[0]) - (r_heel_prev[0] - starting_time[0]);
			double TDS = (r_toe_curr[0] - starting_time[0]) - (r_heel_prev[0] - starting_time[0]);
			*ds_row = IDS + TDS;
		}
		else {                                    // Stride starts with the left leg 
			double IDS = (r_toe_prev[0] - starting_time[0]) - (l_heel_pre[0] - starting_time[0]);
			double TDS = (l_toe_curr[0] - starting_time[0]) - (r_heel_prev[0] - starting_time[0]);
			*ds_row = IDS + TDS;
		}
		VECTOR_ADD(*ds, ds_row);
	}
}


/*
 *	Function: compute_stridelength
 *	------------------------------
 *	Computes the stridelength 
 *
 *	stridelength: A vector that will be populated with the stridelength data
 *	instants: A vector containing the temporal instants data 
 *	acc: A vector containing the acc data
 *	gyr: A vector containing the gyr data
 *	sample_f: The sample freq of the inertial sensors (WIMU's)
 *	segment: The length from the shank WIMU to the ground (cm)
*/
void compute_stridelength(vector *stridelength, vector *instants, vector *acc, vector *gyr, double sample_f, int segment) {

	int step_counter = 0;
	int j = 0;
	double *starting_time = VECTOR_GET(*gyr, double*, 0);
	for (j = 1; j < instants->total; j++, step_counter++) {
		double *shank_curr = VECTOR_GET(*instants, double*, j);
		double *shank_prev = VECTOR_GET(*instants, double*, j - 1);
		double sv1 = shank_prev[5];
		double sv2 = shank_curr[5];
		int l = (int)(sv2 - sv1) + 1;

		if (l == 1) {
			continue;  //Skip computation when adjacent shank vertical indexes are the same 
		}
		double max = 0;
		double min = MIN;
		/* Set up data structure */
		double **vel_y = (double**)calloc(l, sizeof(double*));
		double **vel_z = (double**)calloc(l, sizeof(double*));
		double **s_y = (double**)calloc(l, sizeof(double*));
		double **s_z = (double**)calloc(l, sizeof(double*));
		int x = 0;
		for (x = 0; x < l; x++) {
			double *row_1 = (double*)calloc(instants->total, sizeof(double));
			double *row_2 = (double*)calloc(instants->total, sizeof(double));
			double *row_3 = (double*)calloc(instants->total, sizeof(double));
			double *row_4 = (double*)calloc(instants->total, sizeof(double));
			vel_y[x] = row_1;
			vel_z[x] = row_2;
			s_y[x] = row_3;
			s_z[x] = row_4;
		}
		double *vel_os_y = (double*)calloc(instants->total, sizeof(double));
		double *s_tot_y = (double*)calloc(instants->total, sizeof(double));
		double *s_tot_z = (double*)calloc(instants->total, sizeof(double));
		double *angle = (double*)calloc(l, sizeof(double));
		double *acc_s_y = (double*)calloc(l, sizeof(double));
		double *acc_s_z = (double*)calloc(l, sizeof(double));

		int i = 0;
		for (i = 1; i < l; i++) {
			double *shank_curr_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 1);
			double *shank_prev_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 2);
			angle[i] = angle[i - 1] + shank_curr_time[1] * ((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0]));
		}

		for (i = 0; i < l; i++) {
			double *acc_row = VECTOR_GET(*acc, double*, (int)sv1 + i - 1);
			double r_mat[2][2] = {
				{ cos(angle[i]), -sin(angle[i]) },
				{ sin(angle[i]), cos(angle[i]) }
			};
			double acc_mat[2][1] = {
				{ acc_row[2] },
				{ acc_row[3] }
			};
			double product_mat[2][1] = { 0 };
			int c = 0;
			int d = 0;
			int k = 0;
			double sum = 0;
			for (c = 0; c < 2; c++) {
				for (d = 0; d < 1; d++) {
					for (k = 0; k < 2; k++) {
						sum = sum + r_mat[c][k] * acc_mat[k][d];
					}
					product_mat[c][d] = sum * 9.81;
					sum = 0;
				}
			}
			product_mat[1][0] = product_mat[1][0] - 9.81;

			acc_s_y[i] = product_mat[0][0];
			acc_s_z[i] = product_mat[1][0];
		}
		double *gyr_prev_row = VECTOR_GET(*gyr, double*, (int)sv1);
		vel_y[0][step_counter] = fabs(gyr_prev_row[1]) * (double)segment / 100;
		for (i = 1; i < l; i++) {
			double *shank_curr_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 1);
			double *shank_prev_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 2);
			vel_y[i][step_counter] = vel_y[i - 1][step_counter] + ((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0])) * (acc_s_y[i] + acc_s_y[i - 1]) / 2;
			vel_z[i][step_counter] = vel_z[i - 1][step_counter] + ((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0])) * (acc_s_z[i] + acc_s_z[i - 1]) / 2;
		}
		for (i = 1; i < l; i++) {
			vel_z[i][step_counter] = vel_z[i][step_counter] - vel_z[l - 1][step_counter] * i / l;
		}
		double *gyr_curr_row = VECTOR_GET(*gyr, double*, (int)sv2);
		vel_os_y[step_counter] = fabs(gyr_curr_row[1]) * (double)segment / 100;
		for (i = 1; i < l; i++) {
			vel_y[i][step_counter] = vel_y[i][step_counter] - (vel_y[l - 1][step_counter] - vel_os_y[step_counter]) * i / l;
		}
		for (i = 1; i < l; i++) {
			double *shank_curr_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 1);
			double *shank_prev_time = VECTOR_GET(*gyr, double*, (int)sv1 + i - 2);
			s_y[i][step_counter] = s_y[i - 1][step_counter] + (vel_y[i - 1][step_counter] * ((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0])) + 0.5 * acc_s_y[i] * pow((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0]), 2));
			s_z[i][step_counter] = s_z[i - 1][step_counter] + (vel_z[i - 1][step_counter] * ((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0])) + 0.5 * acc_s_z[i] * pow((shank_curr_time[0] - starting_time[0]) - (shank_prev_time[0] - starting_time[0]), 2));
		}
		for (i = 1; i < l; i++) {
			s_z[i][step_counter] = s_z[i][step_counter] - s_z[l - 1][step_counter] * i / l;
			if (s_z[i][step_counter] > max)
				max = s_z[i][step_counter];
			if (s_z[i][step_counter] < min)
				min = s_z[i][step_counter];
		}
		s_tot_y[step_counter] = s_y[l - 1][step_counter];
		for (i = 1; i < l; i++) {
			s_tot_z[step_counter] = s_tot_z[step_counter] + fabs(s_z[i][step_counter] - s_z[i - 1][step_counter]);
		}
		double *stride = (double*)calloc(3, sizeof(double)); // New row containing [SL0, SL1, CLEAR]
		stride[0] = sqrt(pow(s_tot_y[step_counter], 2) + pow(s_tot_z[step_counter], 2));
		stride[1] = j - 1;
		stride[2] = max - min;
		VECTOR_ADD(*stridelength, stride);

		/* Free allocated space */
		for (x = 0; x < l; x++) {
			free(vel_y[x]);
			free(vel_z[x]);
			free(s_y[x]);
			free(s_z[x]);
		}
		free(vel_y);
		free(vel_z);
		free(s_y);
		free(s_z);
		free(vel_os_y);
		free(s_tot_y);
		free(s_tot_z);
		free(angle);
		free(acc_s_y);
		free(acc_s_z);
	}
}

/*
 *	Function: get_peaks
 *	-------------------
 *	Counts the number of peaks in the gyro X 
 *	axis data
 *
 *	gyr: The gyro vector 
 *	start: The row to begin counting peaks
 *	Returns: The number of peaks
*/
int *get_peaks(vector *gyr, int start, int *num_peaks) {

	int i = 0;
	int length = 0;

	/* Count number of peaks to create storage */
	for (i = start; i < gyr->total - 1; i++) {
		double *pre_row = VECTOR_GET(*gyr, double*, i - 1);
		double *curr_row = VECTOR_GET(*gyr, double*, i);
		double *nxt_row = VECTOR_GET(*gyr, double*, i + 1);
		if (curr_row[1] >= PEAK_HEIGHT) {
			if (curr_row[1] > pre_row[1] && curr_row[1] >= nxt_row[1]) {
				length++;
			}
		}
	}

	int *peaks = (int *)malloc(length * sizeof(int)); // Create storage for peak indices 
	int peaks_index = 0;

	/* Store the index of each peak */
	for (i = start; i < gyr->total - 1; i++) {
		double *pre_row = VECTOR_GET(*gyr, double*, i - 1);
		double *curr_row = VECTOR_GET(*gyr, double*, i);
		double *nxt_row = VECTOR_GET(*gyr, double*, i + 1);
		if (curr_row[1] >= PEAK_HEIGHT) {
			if (curr_row[1] > pre_row[1] && curr_row[1] >= nxt_row[1]) {
				peaks[peaks_index] = i;
				peaks_index++;
			}
		}
	}
	*num_peaks = length;
	return (peaks);
}

/* 
 *	Function: sample_freq
 *	---------------------
 *	Computes the sample frequency of the 
 *	data set
 *
 *	vec: The data set with time stamp in the [3]
 *	     position
 *	Returns: The sample frequency
 */
double sample_freq(vector *vec) {
	double freq;
	double *samp_tmp = VECTOR_GET(*vec, double*, vec->total - 1);
	freq = samp_tmp[3];
	samp_tmp = VECTOR_GET(*vec, double*, 0);
	freq = freq - samp_tmp[3];
	freq = vec->total / freq;
	return freq;
}

/*
 *	Function: print_gait
 *	--------------------
 *	Auxiliary function to print the gait
 *	parameters to a .txt file
 *
 *	g: A structure containing necessary 
 *	vectors
 *	
 *	Returns: void
 */
void print_gait(gait *g) {

	int i = 0;                     // Loop Counter
	errno_t err;
	FILE *file_r;                  // Right File
	FILE *file_l;                  // Left File

	char buff_r[100];
	char buff_l[100];
	sprintf_s(buff_r, sizeof(buff_r), "gait_parameters_right_leg.txt");     // Names of output files
	sprintf_s(buff_l, sizeof(buff_l), "gait_parameters_left_leg.txt");

	err = fopen_s(&file_r, buff_r, "w");
	err = fopen_s(&file_l, buff_l, "w");

	/* Print Header */
	fprintf(file_r, "R GCTs (s)\t");
	fprintf(file_r, "R Stance Phase (s)\t");
	fprintf(file_r, "R Swing Phase (s)\t");
	fprintf(file_r, "R Stride Length (m)\t");
	fprintf(file_r, "R Stride Speed (m/s)\t");
	fprintf(file_r, "R Clearence (m)\t");
	fprintf(file_r, "R Cadence (GC/min)\t");
	fprintf(file_r, "R Gate Variability\n");

	fprintf(file_l, "L GCTs (s)\t");
	fprintf(file_l, "L Stance Phase (s)\t");
	fprintf(file_l, "L Swing Phase (s)\t");
	fprintf(file_l, "L Stride Length (m)\t");
	fprintf(file_l, "L Stride Speed (m/s)\t");
	fprintf(file_l, "L Clearence (m)\t");
	fprintf(file_l, "L Cadence (GC/min)\t");
	fprintf(file_l, "L Gate Variability\n");

	/* Print right gait parameters */
	int total = g->sp_l->total;
	for (i = 0; i < total; i++) {
		double *tmp_gct = VECTOR_GET(*g->intervals_r, double *, i);
		double *tmp_sl = VECTOR_GET(*g->stridelength_r, double *, i);
		double *tmp_sp = VECTOR_GET(*g->sp_r, double *, i);
		fprintf(file_r, "%f\t", tmp_gct[0]);
		fprintf(file_r, "%f\t", tmp_gct[2]);
		fprintf(file_r, "%f\t", tmp_gct[1]);
		fprintf(file_r, "%f\t", tmp_sl[0]);
		fprintf(file_r, "%f\t", tmp_sp[0]);
		fprintf(file_r, "%f\t", tmp_sl[1]);
		if (i == 0) {
			fprintf(file_r, "%f\t", g->cadence_r);
			fprintf(file_r, "%f\t\n", g->gate_v_r);
		}
		else
			fprintf(file_r, "\n");
	}

	total = g->sp_l->total;

	/* Print left gait parameters */
	for (i = 0; i < total; i++) {
		double *tmp_gct = VECTOR_GET(*g->intervals_l, double *, i);
		double *tmp_sl = VECTOR_GET(*g->stridelength_l, double *, i);
		double *tmp_sp = VECTOR_GET(*g->sp_l, double *, i);
		fprintf(file_l, "%f\t", tmp_gct[0]);
		fprintf(file_l, "%f\t", tmp_gct[2]);
		fprintf(file_l, "%f\t", tmp_gct[1]);
		fprintf(file_l, "%f\t", tmp_sl[0]);
		fprintf(file_l, "%f\t", tmp_sp[0]);
		fprintf(file_l, "%f\t", tmp_sl[1]);
		if (i == 0) {
			fprintf(file_l, "%f\t", g->cadence_l);
			fprintf(file_l, "%f\t\n", g->gate_v_l);
		}
		else
			fprintf(file_l, "\n");
	}

	fclose(file_r);
	fclose(file_l);
}
