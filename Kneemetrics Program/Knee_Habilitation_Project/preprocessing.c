/*
*	Class: preprocessing.c
*	-----------------------------
*	Provides various filtering and interpolation functions to process WIMU data,
*	including a low pass and median filter
*/

#include "preprocessing.h"


/*
 *	Function: lininterp2:
 *	---------------------
 *	Interpolates timestamp data that dont meet the threshold and adds
 *	precise data samples to the gaps
 *
 *	euler: The vector contianing sample data
 *	Returns: 0 if the process was sucessful, 1 if an error occured
*/
int lininterp2(vector *euler) {

	/* Member Initilation */
	double *first = VECTOR_GET(*euler, double *, 0);
	double *prev = 0;
	double *curr = 0;
	double diff = 0;
	double mean = 0;
	/* Find the mean of the timestamp data */
	int i = 0;
	for (i = 1; i < euler->total; i++) {
		prev = VECTOR_GET(*euler, double *, i - 1);
		curr = VECTOR_GET(*euler, double *, i);
		curr[3] = curr[3] - first[3];
		diff = diff + (curr[3] - prev[3]);
	}
	mean = diff / (euler->total);	//	calculate mean 
	/*	Preform interpolation	*/
	i = 1;
	int f = 0;
	while (i < euler->total) {
		prev = VECTOR_GET(*euler, double *, i - 1);
		curr = VECTOR_GET(*euler, double *, i);
		diff = curr[3] - prev[3];
		if (diff > 0.025) {	// Threshold values (can be changed)
			int n = (int)floor(diff / mean);	// Number of values required for interpolation	
			int j = 0;
			/* Spacing for each data point */
			double roll_space = (curr[0] - prev[0]) / (n - 1);
			double pitch_space = (curr[1] - prev[1]) / (n - 1);
			double yaw_space = (curr[2] - prev[2]) / (n - 1);
			double time_space = (curr[3] - prev[3]) / (n - 1);
			for (j = 0; j < (n - 2); j++) {
				double *new_row = malloc(4 * sizeof(double));
				if (new_row == NULL) {
					printf("could'nt allocate memory!");
					return 1;
				}
				new_row[0] = prev[0] + (j + 1)*roll_space;
				new_row[1] = prev[1] + (j + 1)*pitch_space;
				new_row[2] = prev[2] + (j + 1)*yaw_space;
				new_row[3] = prev[3] + (j + 1)*time_space;
				VECTOR_INSERT(*euler, i + j - 1, new_row);
			}
			i = i + n - 1;
		}
		i++;
	}
	first[3] = first[3] - first[3];	//	Zero first timestamp
	return 0;
}

/*	
 *	Function: lininterp1
 *	--------------------
 *	Preforms a linear interpolation on the timestamps of the samples
 *	to account for sensor and read/write errors
 *
 *	euler: The matrix containing sample data
 *	Returns: 0 if the process was successful, 1 if an error occured
*/
int lininterp1(vector *euler) {

	/*	Member initilation list */
	double *first = 0;
	double *time_1 = 0;
	double *time_2 = 0;
	double diff_1 = 0;
	double first_time = 0;

	/* Preform linear interpolation */
	int i = 0;
	for (i = 0; i < (euler->total - 9); i++) {
		first = VECTOR_GET(*euler, double *, 0);
		first_time = first[3];	//	Retrieve first timestamp
		time_1 = VECTOR_GET(*euler, double *, i);
		time_2 = VECTOR_GET(*euler, double *, i + 1);
		diff_1 = (time_2[3] - first_time) - (time_1[3] - first_time);	//	First/second timestamp difference

		if (diff_1 > 0.2 || diff_1 < 0) {	//	Threshold values (can be changed later)
			int j = i;
			while (diff_1 > 0.2 || diff_1 < 0) {
				j++;
				time_2 = VECTOR_GET(*euler, double *, j + 1);	//	time_ 2 will mark end of the interpolation 
				diff_1 = (time_2[3] - first_time) - (time_1[3] - first_time);
			}
			int n = j - i + 2;	// Number of values required for interpolation

			printf("I: %d, J: %d\n", i, j);
			double roll_gap = time_2[0] - time_1[0];
			double pitch_gap = time_2[1] - time_1[1];
			double yaw_gap = time_2[2] - time_1[2];
			double spacing = (time_2[3] - time_1[3]) / (n - 1);

			int x = 0;
			for (x = 0; x < (n - 1); x++) {	//	Create and set new rows
				double new_row[4] = { 0 };
				if (new_row == NULL) {
					printf("could'nt allocate memory!");
					return 1;
				}
				new_row[0] = time_1[0] + roll_gap;
				new_row[1] = time_1[1] + pitch_gap;
				new_row[2] = time_1[2] + yaw_gap;
				new_row[3] = time_1[3] + (x + 1)*spacing;
				VECTOR_SET(*euler, i + x + 1, new_row);
			}
		}
	}
	return 0;

}
/*
 *	Function: d_coeff
 *	-----------------
 *	Auxiliary function to find d coefficients for low pass butterworth filter
 *	
 *	n: Filter order
 *	fcf: Cutoff frequency
 *	dcof: array pointer that gets updated with d coefficients
*/
void d_coeff(int n, double fcf, double *dcof) {
	double *dcof_orig;
	dcof_orig = dcof_bwlp(n, fcf);
	if (dcof == NULL)
	{
		perror("Unable to calculate d coefficients");
		return;
	}
	int i = 0;
	for (i = 0; i <= n; i++) {
		dcof[i] = dcof_orig[i];
	}
	return;
}

/*
*	Function: c_coeff
*	-----------------
*	Auxiliary function to find c coefficients for low pass butterworth filter
*
*	n: Filter order
*	fcf: Cutoff frequency
*	sff: Identifies whether to scale or not 
*	ccof: array pointer that gets updated with c coefficients
*/
void c_coeff(int n, double fcf, int sff, double *ccof) {
	int *ccof_orig;      // unprocessed c coefficients
	ccof_orig = ccof_bwlp(n);
	if (ccof_orig == NULL)
	{
		perror("Unable to calculate c coefficients");
		return;
	}
	double sf = sf_bwlp(n, fcf); /* scaling factor for the c coefficients */
	int i = 0;
	for (i = 0; i <= n; ++i)
		if (sff == 1)
			ccof[i] = (double)ccof_orig[i] * sf;
		else
			ccof[i] = (double)ccof_orig[i];
	free(ccof_orig);
	return;
}

/*
 *	Function: filter_vector
 *	--------------------
 *	Performs a low pass filter on the vectors columns
 *
 *	data: The vector to be filtered 
 *	order: The filter order
 *	cut: The cutoff frequency as a fraction of PI
 */
void filter_vector(vector *data, int order, double cut) {
	double *dcof = malloc((order + 1) * sizeof(double));     // d coefficients
	double *ccof = malloc((order + 1) * sizeof(double));;        // c coefficients
	d_coeff(order, cut, dcof);
	c_coeff(order, cut, SCALE_ON, ccof);                      // Scaling is on by default
	filter_vector_helper(data, dcof, ccof, order + 1, order + 1);    // call the helper function to filter
	free(dcof);
	free(ccof);
}

/*
*	Function: filter_data
*	--------------------
*	Performs a low pass filter on the array of doubles
*
*	data: The array to be filtered
*	order: The filter order
*	cut: The cutoff frequency as a fraction of PI
*	size: The size of the array to be filtered
*/
void filter_data(double *data, int order, double cut, int size) {
	double *dcof = malloc((order + 1) * sizeof(double));     // d coefficients
	double *ccof = malloc((order + 1) * sizeof(double));;        // c coefficients
	d_coeff(order, cut, dcof);
	c_coeff(order, cut, SCALE_ON, ccof);                      // Scaling is on by default
	int i = 0;
	filter_data_helper(data, dcof, ccof, order + 1, order + 1, size);    // call the helper function to filter
	free(dcof);
	free(ccof);
}

/*
 *	Function: filter_data_helper
 *	----------------------------
 *	Auxiliary function to perform a low pass filter on the array of doubles
 *
 *	data: The array to be filtered
 *	dcof: Array of d coefficients
 *	ccof: Array of c coefficients
 *	nd: The number of d coefficients
 *	nc: The number of c coefficients
 *	size: The size of the array to be filtered
 */
void filter_data_helper(double *data, double *dcof, double *ccof, int nd, int nc, int size) {

	double *z = calloc(nd, sizeof(double));
	int i = 0;
	for (i = 0; i < nc; i++)
		ccof[i] = ccof[i] / dcof[0];
	for (i = 0; i < nd; i++)
		dcof[i] = dcof[i] / dcof[0];

	double *y = calloc(size, sizeof(double));
	int m = 0;
	int j = 0;
	for (m = 0; m < size; m++) {
		y[m] = ccof[0] * data[m] + z[0];
		for (i = 1; i < nd; i++) {
			z[i - 1] = ccof[i] * data[m] + z[i] - dcof[i] * y[m];
		}
		data[m] = y[m];
	}
	free(z);
	free(y);
}


/*
 *	Function: filter_vector_helper
 *	-----------------------
 *	Auxiliary function to perform a low pass filter on the vectors columns
 *
 *	data: The vector to be filtered
 *	dcof: Array of d coefficients
 *	ccof: Array of c coefficients
 *	nd: The number of d coefficients
 *	nc: The number of c coefficients
 */
void filter_vector_helper(vector *data, double *dcof, double *ccof, int nd, int nc) {

	double *z_x = calloc(nd, sizeof(double));
	double *z_y = calloc(nd, sizeof(double));
	double *z_z = calloc(nd, sizeof(double));
	int i = 0;
	for (i = 0; i < nc; i++)
		ccof[i] = ccof[i] / dcof[0];
	for (i = 0; i < nd; i++)
		dcof[i] = dcof[i] / dcof[0];
	double *y_x = calloc(data->total, sizeof(double));
	double *y_y = calloc(data->total, sizeof(double));
	double *y_z = calloc(data->total, sizeof(double));

	double prev_row[4] = { 0 };

	int m = 0;
	int j = 0;
	for (m = 0; m < data->total; m++) {
		double *x = VECTOR_GET(*data, double*, m);
		double diff = x[0] - prev_row[0];

		if (m > 0 && (diff > 2.0 || diff < 0)) {
			for (j = 0; j < 4; j++)
				x[j] = prev_row[j];
		}
		else {
			for (j = 0; j < 4; j++)
				prev_row[j] = x[j];
			y_x[m] = ccof[0] * x[1] + z_x[0];
			y_y[m] = ccof[0] * x[2] + z_y[0];
			y_z[m] = ccof[0] * x[3] + z_z[0];
			for (i = 1; i < nd; i++) {
				z_x[i - 1] = ccof[i] * x[1] + z_x[i] - dcof[i] * y_x[m];
				z_y[i - 1] = ccof[i] * x[2] + z_y[i] - dcof[i] * y_y[m];
				z_z[i - 1] = ccof[i] * x[3] + z_z[i] - dcof[i] * y_z[m];
			}
			x[1] = y_x[m];
			x[2] = y_y[m];
			x[3] = y_z[m];
		}
	}
	free(z_x);
	free(z_y);
	free(z_z);
	free(y_x);
	free(y_y);
	free(y_z);
}

/*
 *	Function: median_filter_set_up
 *	----------------------------
 *	Sets up a median filter for each column of a vector
 *
 *	vec: The vector to be filtered
 */
void median_filter_set_up(vector* vec) {

	/* Set up each colums filter data */
	filter f_x = { 0 };              // X column
	f_x.datpoint = f_x.buffer;
	f_x.small.point = NULL;
	f_x.small.value = STOPPER;
	f_x.big.point = &(f_x.small);
	f_x.big.value = 0;
	filter f_y = { 0 };              // Y column
	f_y.datpoint = f_y.buffer;
	f_y.small.point = NULL;
	f_y.small.value = STOPPER;
	f_y.big.point = &(f_y.small);
	f_y.big.value = 0;
	filter f_z = { 0 };              // Z column
	f_z.datpoint = f_z.buffer;
	f_z.small.point = NULL;
	f_z.small.value = STOPPER;
	f_z.big.point = &(f_z.small);
	f_z.big.value = 0;

	/* Iterate through the vector data struct and perform the 
	 * median filtering 
	 */
	for (int i = 0; i < vec->total; i++) {
		double *tmp = VECTOR_GET(*vec, double*, i);
		double new_x = median_filter(tmp[1], &f_x);
		double new_y = median_filter(tmp[2], &f_y);
		double new_z = median_filter(tmp[3], &f_z);
		tmp[1] = new_x;
		tmp[2] = new_y;
		tmp[3] = new_z;
	}
}

/*
 *	Function: median_filter
 *	-----------------------
 *	Performs a median filter on numerical data
 *
 *	datum: The next data point entering the filter
 *	filt: Filter struct that holds a columns filter information
 *	Returns: The filtered data point 
 */
double median_filter(double datum, filter* filt)
{

	struct pair *successor;                              /* Pointer to successor of replaced data item */
	struct pair *scan;                                   /* Pointer used to scan down the sorted list */
	struct pair *scanold;                                /* Previous value of scan */
	struct pair *median;                                 /* Pointer to median */
	double i;

	if (datum == STOPPER)
	{
		datum = STOPPER + 1;                             /* No stoppers allowed. */
	}

	if ((++filt->datpoint - filt->buffer) >= MEDIAN_FILTER_SIZE)
	{
		filt->datpoint = filt->buffer;                               /* Increment and wrap data in pointer.*/
	}

	filt->datpoint->value = datum;                           /* Copy in new datum */
	successor = filt->datpoint->point;                       /* Save pointer to old value's successor */
	median = &(filt->big);                                     /* Median initially to first in chain */
	scanold = NULL;                                    /* Scanold initially null. */
	scan = &(filt->big);                                       /* Points to pointer to first (largest) datum in chain */

													   /* Handle chain-out of first item in chain as special case */
	if (scan->point == filt->datpoint)
	{
		scan->point = successor;
	}
	scanold = scan;                                     /* Save this pointer and   */
	scan = scan->point;                                /* step down chain */

													   /* Loop through the chain, normal loop exit via break. */
	for (i = 0; i < MEDIAN_FILTER_SIZE; ++i)
	{
		/* Handle odd-numbered item in chain  */
		if (scan->point == filt->datpoint)
		{
			scan->point = successor;                      /* Chain out the old datum.*/
		}

		if (scan->value < datum)                        /* If datum is larger than scanned value,*/
		{
			filt->datpoint->point = scanold->point;             /* Chain it in here.  */
			scanold->point = filt->datpoint;                    /* Mark it chained in. */
			datum = STOPPER;
		};

		/* Step median pointer down chain after doing odd-numbered element */
		median = median->point;                       /* Step median pointer.  */
		if (scan == &(filt->small))
		{
			break;                                      /* Break at end of chain  */
		}
		scanold = scan;                               /* Save this pointer and   */
		scan = scan->point;                           /* step down chain */

													  /* Handle even-numbered item in chain.  */
		if (scan->point == filt->datpoint)
		{
			scan->point = successor;
		}

		if (scan->value < datum)
		{
			filt->datpoint->point = scanold->point;
			scanold->point = filt->datpoint;
			datum = STOPPER;
		}

		if (scan == &(filt->small))
		{
			break;
		}

		scanold = scan;
		scan = scan->point;
	}
	return median->value;
}


