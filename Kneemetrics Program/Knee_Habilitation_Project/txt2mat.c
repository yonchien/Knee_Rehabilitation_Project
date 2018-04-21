/*
*	Class: txt2mat.c
*	----------------
*	Provides functions to import data from .txt files and store them 
*	in the underlying vector data structure
*/

#include "txt2mat.h"

/*
*	Function: open_prev_files
*	--------------------
*	Opens .txt files containing WIMU data to be imported
*	
*	f: The Prev_Files struct to be updated
*	number: The WIMU number
*	Returns: 0 if the files were opened correctly, 1 if 
*	         there was an error opening the files
*/
int open_prev_files(Prev_Files *f, int number) {

	FILE *file_acc;
	FILE *file_gyr;
	FILE *file_ts;
	errno_t err = 0;

	/* Create appropriate file paths for text files */
	char acc_file_head[] = "input/MPUacc_WIMU_";
	char gyr_file_head[] = "input/MPUgyr_WIMU_";
	char time_file_head[] = "input/timestamp_WIMU_";
	char file_ext[] = ".txt";
	int length = 1;
	int temp_num = number;
	while (temp_num>9) { length++; temp_num /= 10; }
	char acc_filename[200];
	char gyr_filename[200];
	char time_filename[200];
	sprintf_s(acc_filename, sizeof(acc_filename), "%s%d%s", acc_file_head, number, file_ext);
	sprintf_s(gyr_filename, sizeof(gyr_filename), "%s%d%s", gyr_file_head, number, file_ext);
	sprintf_s(time_filename, sizeof(time_filename), "%s%d%s", time_file_head, number, file_ext);

	/* Open text files */
	err = fopen_s(&file_acc, acc_filename, "r");
	if (err != 0)
		return 1;
	err = fopen_s(&file_gyr, gyr_filename, "r");
	if (err != 0)
		return 1;
	err = fopen_s(&file_ts, time_filename, "r");
	if (err != 0)
		return 1;

	/* Find number of lines in the file */
	char buffer[100];
	int lines = 0;
	while (fgets(buffer, 100, file_ts) != NULL)
		lines++;
	rewind(file_ts);

	/* Set File struct members */
	f->file_num = number;
	f->num_lines = lines;
	f->file_acc = file_acc;
	f->file_gyr = file_gyr;
	f->file_ts = file_ts;

	return 0;
}



/*
*	Function: open_sens_files
*	--------------------
*	Opens .txt files containing WIMU data to be imported
*
*	f: The Sens_Files struct to be updated
*	Returns: 0 if the files were opened correctly, 1 if
*	         there was an error opening the files
*/
int open_sens_files(Sens_Files *f) {

	WIMU_Files *vec = f->all_files;
	errno_t err = 0;

	char file_head_s[] = "input/SENS_";
	char file_head_e[] = "input/EULER_";
	char file_ext[] = ".txt";

	char buffer_lines[200];
	int lines = 0;
	char buffer_s[200];
	int record = 0;

	/* Open SENS files */
	sprintf_s(buffer_s, sizeof(buffer_s), "%s%d%s", file_head_s, f->sensor_nums_r[0], file_ext);        // Right Thigh
	err = fopen_s(&f->sens_rt, buffer_s, "r");
	if (err != 0)
		return 1;
	while (fgets(buffer_lines, sizeof(buffer_lines), f->sens_rt) != NULL && record != (f->record_number + 1)) {      // Count Lines
		if (record == f->record_number)
			lines++;
		if (buffer_lines[1] == '-')
			record++;
	}
	if (lines == 0){
		printf("Could not find record number!");
		return 1;
	}
	record = 0;
	rewind(f->sens_rt);
	vec->line_number_rt = lines;
	lines = 0;

	sprintf_s(buffer_s, sizeof(buffer_s), "%s%d%s", file_head_s, f->sensor_nums_r[1], file_ext);        // Righ Shank
	err = fopen_s(&f->sens_rs, buffer_s, "r");
	if (err != 0)
		return 1;
	while (fgets(buffer_lines, sizeof(buffer_lines), f->sens_rs) != NULL && record != (f->record_number + 1)) {      // Count Lines
		if(record == f->record_number)
			lines++;
		if (buffer_lines[1] == '-')
			record++;
	}
	if (lines == 0) {
		printf("Could not find record number!");
		return 1;
	}
	record = 0;
	rewind(f->sens_rs);
	vec->line_number_rs = lines;
	lines = 0;
	sprintf_s(buffer_s, sizeof(buffer_s), "%s%d%s", file_head_s, f->sensor_nums_l[0], file_ext);        // Left Thigh 
	err = fopen_s(&f->sens_lt, buffer_s, "r");
	if (err != 0)
		return 1;
	while (fgets(buffer_lines, sizeof(buffer_lines), f->sens_lt) != NULL && record != (f->record_number + 1)) {     // Count Lines
		if (record == f->record_number)
			lines++;
		if (buffer_lines[1] == '-')
			record++;
	}
	if (lines == 0) {
		printf("Could not find record number!");
		return 1;
	}
	record = 0;
	rewind(f->sens_lt);
	vec->line_number_lt = lines;
	lines = 0;
	
	sprintf_s(buffer_s, sizeof(buffer_s), "%s%d%s", file_head_s, f->sensor_nums_l[1], file_ext);        // Left Shank
	err = fopen_s(&f->sens_ls, buffer_s, "r");
	if (err != 0)
		return 1;
	while (fgets(buffer_lines, sizeof(buffer_lines), f->sens_ls) != NULL && record != (f->record_number + 1)) {    // Count Lines
		if (record == f->record_number)
			lines++;
		if (buffer_lines[1] == '-')
			record++;
	}
	if (lines == 0) {
		printf("Could not find record number!");
		return 1;
	}
	record = 0;
	rewind(f->sens_ls);
	vec->line_number_ls = lines;
	lines = 0;

	if (f->euler == 0) {
		/* Open EULER files */
		char buffer_e[200];
		sprintf_s(buffer_e, sizeof(buffer_e), "%s%d%s", file_head_e, f->sensor_nums_r[0], file_ext);   // Right Thigs
		err = fopen_s(&f->eul_rt, buffer_e, "r");
		if (err != 0)
			return 1;
		sprintf_s(buffer_e, sizeof(buffer_e), "%s%d%s", file_head_e, f->sensor_nums_r[1], file_ext);   // Right Shank
		err = fopen_s(&f->eul_rs, buffer_e, "r");
		if (err != 0)
			return 1;
		sprintf_s(buffer_e, sizeof(buffer_e), "%s%d%s", file_head_e, f->sensor_nums_l[0], file_ext);  // Left Thigh
		err = fopen_s(&f->eul_lt, buffer_e, "r");
		if (err != 0)
			return 1;
		sprintf_s(buffer_e, sizeof(buffer_e), "%s%d%s", file_head_e, f->sensor_nums_l[1], file_ext);  // Left Shank
		err = fopen_s(&f->eul_ls, buffer_e, "r");
		if (err != 0)
			return 1;
	}
	return 0;                                          // All files opened sucessfully 
}

/*
 *	Function scan_prev_files:
 *	-----------------------
 *	Scans time_stamp, acc, gyr, txt files for ALL EXERCISES 
 *	on a single WIMU and stores
 *	the data in a vector data structure
 *	
 *	f: The Files struct containing file and vector data
 *	leg_num: Leg specifier: 0 = left, 1 = right
*/
void scan_prev_files(Prev_Files* f, int leg_num) {

	/* Access File struct members */
	vector *acc_vec = f->acc_vec;
	vector *gyr_vec = f->gyr_vec;
	FILE *file_ts = f->file_ts;
	FILE *file_acc = f->file_acc;
	FILE *file_gyr = f->file_gyr;

	/* Skip header line in acc/gyr txt files */
	char buffer[100];
	fgets(buffer, 100, file_acc);
	fgets(buffer, 100, file_gyr);

	/* Scan and add data to each vector */
	double ts_num = 0.0, garbage = 0.0;
	double acc_x = 0.0, acc_y = 0.0, acc_z = 0.0, acc_garbage = 0.0;
	double gyr_x = 0.0, gyr_y = 0.0, gyr_z = 0.0, gyr_garbage = 0.0;
	double zero = 0.0;
	int zero_count = 0, max_zeros = 8, vec_index = 0, rec_index = 0;
	/* Scanning time_stamp file */
	while (fscanf_s(file_ts, "%lf\t%lf", &ts_num, &garbage) != EOF) {
		/* Checks for more than eight adjacent zero rows and removes extras */
		if (compare_double(ts_num, zero) == 1)
			zero_count++;
		else
			zero_count = 0;
		/* Add last index of all zero blocks to record_num array */
		if (zero_count == 8) {
			f->record_num[rec_index] = vec_index;
			rec_index++;
		}
		if (zero_count > max_zeros)
			continue;
		double *acc = (double*)malloc(4 * sizeof(double));
		double *gyr = (double*)malloc(4 * sizeof(double));
		acc[0] = ts_num;
		gyr[0] = ts_num;
		/* Scanning acc txt file */
		/* Right Leg */
		if (leg_num == 1) {
			if (fscanf_s(file_acc, "%lf\t%lf\t%lf\t%lf", &acc_x, &acc_y, &acc_z, &acc_garbage) != EOF) {
				acc[1] = acc_z;
				acc[2] = -acc_x;
				acc[3] = -acc_y;
			}
			/* Scanning gyr txt file */
			if (fscanf_s(file_gyr, "%lf\t%lf\t%lf\t%lf", &gyr_x, &gyr_y, &gyr_z, &gyr_garbage) != EOF) {
				gyr[1] = gyr_z * PI / 180;             // from deg/s to rad/s 
				gyr[2] = -gyr_x * PI / 180;            // from deg/s to rad/s 
				gyr[3] = -gyr_y * PI / 180;            // from deg/s to rad/s 
			}
			VECTOR_ADD(*acc_vec, acc);
			VECTOR_ADD(*gyr_vec, gyr);
			vec_index++;

		}
		/* Left Leg */
		else {
			if (fscanf_s(file_acc, "%lf\t%lf\t%lf\t%lf", &acc_x, &acc_y, &acc_z, &acc_garbage) != EOF) {
				acc[1] = -acc_z;
				acc[2] = acc_x;
				acc[3] = -acc_y;
			}
			/* Scanning gyr txt file */
			if (fscanf_s(file_gyr, "%lf\t%lf\t%lf\t%lf", &gyr_x, &gyr_y, &gyr_z, &gyr_garbage) != EOF) {
				gyr[1] = -gyr_z * PI / 180;            // from deg/s to rad/s
				gyr[2] = gyr_x * PI / 180;             // from deg/s to rad/s
				gyr[3] = -gyr_y * PI / 180;            // from deg/s to rad/s
			}
			VECTOR_ADD(*acc_vec, acc);
			VECTOR_ADD(*gyr_vec, gyr);
			vec_index++;
		}
	}
	/* Fill empty indices of record_num with zero*/
	while (rec_index < 100) {
		f->record_num[rec_index] = 0;
		rec_index++;
	}
	return;
}


/*
*	Function scan_sens_files:
*	-----------------------
*	Scans SENS and EULER data files and stores
*	the data in vector data structures.
*
*	f: The Sens_Files struct containing the files and vector data
*/
void scan_sens_files(Sens_Files *f) {

	WIMU_Files *vec = f->all_files;
	int record = 0;
	int line_num = 0;
	/* Skip header line in SENS .txt files */
	char buffer[500];
	int i = 0;
	int j = 0;

	/* Scan and add data to the vectors */
	double acc[3] = { 0 };
	double gyr[3] = { 0 };
	double ts_num = 0;
	FILE *curr_file;
	vector *curr_acc;
	vector *curr_gyr;
	/* Scanning right leg SENS files */
	for (i = 0; i < 2; i++) {
		line_num = (i == 0) ? vec->line_number_rt : vec->line_number_rs;
		curr_file = (i == 0) ? f->sens_rt : f->sens_rs;
		curr_acc = (i == 0) ? vec->acc_rt : vec->acc_rs;
		curr_gyr = (i == 0) ? vec->gyr_rt : vec->gyr_rs;
		while (fgets(buffer, sizeof(buffer), curr_file) != NULL && record != f->record_number) {
			if (buffer[1] == '-')
				record++;
		}
		record = 0;
		for (j = 0; j < 2; j++)
			fgets(buffer, sizeof(buffer), curr_file);
		for (j = 0; j < line_num; j++){
			fscanf_s(curr_file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", &acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], &ts_num);
			double *acc_row = malloc(4 * sizeof(double));
			double *gyr_row = malloc(4 * sizeof(double));
			acc_row[0] = ts_num / 1000;
			acc_row[1] = acc[2] * 2 / 32768;
			acc_row[2] = acc[0] * 2 / 32768;
			acc_row[3] = acc[1] * 2 / 32768;
			gyr_row[0] = ts_num / 1000;
			gyr_row[1] = gyr[2] * 2000 / 32768 * PI / 180;
			gyr_row[2] = gyr[0] * 2000 / 32768 * PI / 180;
			gyr_row[3] = gyr[1] * 2000 / 32768 * PI / 180;
			VECTOR_ADD(*curr_acc, acc_row);
			VECTOR_ADD(*curr_gyr, gyr_row);
		}
	}
	/* Scanning left leg SENS files */
	for (i = 0; i < 2; i++) {
		line_num = (i == 0) ? vec->line_number_lt : vec->line_number_ls;
		curr_file = (i == 0) ? f->sens_lt : f->sens_ls;
		curr_acc = (i == 0) ? vec->acc_lt : vec->acc_ls;
		curr_gyr = (i == 0) ? vec->gyr_lt : vec->gyr_ls;
		while (fgets(buffer, sizeof(buffer), curr_file) != NULL && record != f->record_number) {
			if (buffer[1] == '-')
				record++;
		}
		record = 0;
		for (j = 0; j < 2; j++)
			fgets(buffer, sizeof(buffer), curr_file);
		for(j = 0; j < line_num; j++){
			fscanf_s(curr_file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", &acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], &ts_num);
			double *acc_row = malloc(4 * sizeof(double));
			double *gyr_row = malloc(4 * sizeof(double));
			acc_row[0] = ts_num / 1000;
			acc_row[1] = acc[2] * 2 / 32768;
			acc_row[2] = -acc[0] * 2 / 32768;
			acc_row[3] = acc[1] * 2 / 32768;
			gyr_row[0] = ts_num / 1000;
			gyr_row[1] = gyr[2] * 2000 / 32768 * PI / 180;
			gyr_row[2] = -gyr[0] * 2000 / 32768 * PI / 180;
			gyr_row[3] = gyr[1] * 2000 / 32768 * PI / 180;
			VECTOR_ADD(*curr_acc, acc_row);
			VECTOR_ADD(*curr_gyr, gyr_row);
		}
	}
	
	if (f->euler == 0) {

		double eul[3] = { 0 };
		vector *curr_eul;
		/* Scanning all EULER files */
		for (i = 0; i < 4; i++) {
			if (i == 0) {
				curr_file = f->eul_rt;
				curr_eul = vec->eul_rt;
				line_num = vec->line_number_rt;
			}
			if (i == 1) {
				curr_file = f->eul_rs;
				curr_eul = vec->eul_rs;
				line_num = vec->line_number_rs;
			}
			if (i == 2) {
				curr_file = f->eul_lt;
				curr_eul = vec->eul_lt;
				line_num = vec->line_number_lt;
			}
			else {
				curr_file = f->eul_ls;
				curr_eul = vec->eul_ls;
				line_num = vec->line_number_ls;
			}
			while (fgets(buffer, sizeof(buffer), curr_file) != NULL && record != f->record_number) {
				if (buffer[1] == '-')
					record++;
			}
			record = 0;
			for (j = 0; j < 2; j++)
				fgets(buffer, sizeof(buffer), curr_file);
			for (j = 0; j < line_num; j++) {
				fscanf_s(curr_file, "%lf\t%lf\t%lf\t%lf", &eul[0], &eul[1], &eul[2], &ts_num);
				double *eul_row = malloc(4 * sizeof(double));
				for (j = 0; j < 3; j++)
					eul_row[i] = eul[i];
				eul_row[3] = ts_num / 1000;
				VECTOR_ADD(*curr_eul, eul_row);
			}
		}
	}
}



/*
 *	Function: record_size
 *	---------------------
 *	Finds the number of rows that the cut vector will hold
 *
 *	f: The Files struct containing vector data
 *	record_number: The record number to count 
 *	Returns: The number of rows needed 
*/
int record_size(Prev_Files *f, int record_number) {

	vector *acc_vec = f->acc_vec;
	vector *gyr_vec = f->gyr_vec;
	int start_index = 0;
	int end_index = 0;

	if (record_number != 1) {
		start_index = (f->record_num[record_number]) + 1;
		end_index = (f->record_num[record_number]) - 8;
	}
	else {
		start_index = 0;
		end_index = (f->record_num[record_number - 1] - 8);
	}

	return (end_index - start_index + 10);
}

/*
 *	Function: cut_record
 *	--------------------
 *	Cuts the WIMU data based on the record number and frees the 
 *	old vectors data 
 *
 *	f: The Files struct containing vector data
 *	acc_vec_new: The new acc vector that is filled with the cut data
 *	gyr_vec_new: The new gyr vector that is filled with the cut data
 *	record_number: The record number to cut from the old vectors 
 *	Returns: 0 if function exited correctly, 1 if the record number
 *           does not exist 
*/
int cut_record(Prev_Files *f, vector *acc_vec_new, vector *gyr_vec_new,
	int record_number) {

	vector *acc_vec = f->acc_vec;
	vector *gyr_vec = f->gyr_vec;

	/* Find start/end index of record */
	int start_index = 0;
	int end_index = 0;
	if (record_number == 1) {
		start_index = 0;
		end_index = (f->record_num[record_number - 1] - 8);
		f->num_lines = end_index + 1;
	}
	else if (f->record_num[record_number] != 0) {
		start_index = (f->record_num[record_number - 1]) + 1;
		end_index = (f->record_num[record_number]) - 8;
		f->num_lines = end_index - start_index + 1;
	}
	else {
		fprintf(stderr, "Record %d does not exist!", record_number);
		return 1;
	}

	/* Preform deep copy of data from old to new vectors */
	int index = 0;
	for (index = start_index; index <= end_index; index++) {
		double *temp_acc = VECTOR_GET(*acc_vec, double*, index);
		double *temp_gyr = VECTOR_GET(*gyr_vec, double*, index);
		double *acc = (double*)malloc(4 * sizeof(double));
		double *gyr = (double*)malloc(4 * sizeof(double));
		int i = 0;
		for (i = 0; i < 4; i++) {
			acc[i] = temp_acc[i];
			gyr[i] = temp_gyr[i];
		}
		VECTOR_ADD(*acc_vec_new, acc);
		VECTOR_ADD(*gyr_vec_new, gyr);
	}
	/* Free memory allocated for old vectors */
	VECTOR_FREE(*acc_vec);
	VECTOR_FREE(*gyr_vec);
	/* Update struct vector with newly created vectors */
	f->acc_vec = acc_vec_new;
	f->gyr_vec = gyr_vec_new;

	return 0;
}

/*
 *	Function: print_vectors
 *	-----------------------
 *	Auxiliary function that Prints out the acc and gyc vector 
 *	arrays into a .txt file used for testing purposes
 * 
 *	f: The Files struct containing vector data
 *	i: Identification number to be used at the end of the 
 *     .txt filename
*/
void print_vectors(Prev_Files *f, int i) {

	vector *acc_vec = f->acc_vec;
	vector *gyr_vec = f->gyr_vec;

	int index = 0;
	FILE *output_file_acc;
	errno_t err;
	char buff_1[100];
	sprintf_s(buff_1, 100 * sizeof(char), "output_file_acc_%d.txt", i);
	err = fopen_s(&output_file_acc, buff_1, "w");
	for (index = 0; index < VECTOR_TOTAL(*acc_vec); index++) {
		double* temp = VECTOR_GET(*acc_vec, double*, index);
		fprintf(output_file_acc, "%f\t", temp[0]);
		fprintf(output_file_acc, "%f\t", temp[1]);
		fprintf(output_file_acc, "%f\t", temp[2]);
		fprintf(output_file_acc, "%f\t\n", temp[3]);
	}
	fclose(output_file_acc);

	FILE *output_file_gyr;
	char buff_2[100];
	sprintf_s(buff_2, 100 * sizeof(char), "output_file_gyr_%d.txt", i);
	err = fopen_s(&output_file_gyr, buff_2, "w");
	for (index = 0; index < VECTOR_TOTAL(*gyr_vec); index++) {
		double* temp = VECTOR_GET(*gyr_vec, double*, index);
		fprintf(output_file_gyr, "%f\t", temp[0]);
		fprintf(output_file_gyr, "%f\t", temp[1]);
		fprintf(output_file_gyr, "%f\t", temp[2]);
		fprintf(output_file_gyr, "%f\t\n", temp[3]);
	}
	fclose(output_file_gyr);

	return;
}

/*
 *	Function: compare_double
 *	------------------------
 *	Compares double f1 with double f2 for equality 
 *	
 *	f1: first double to compare
 *	f2: second double to compare
 *	Returns: 1 if they are equal and 0 if they are not equal
*/
int compare_double(double f1, double f2) {
	double precision = 0.00001;
	if (((f1 - precision) < f2) && ((f1 + precision) > f2))
		return 1;
	else
		return 0;
}
