/*
 *	Class: main.c 
 *	---------------
 *	Contains all function calls for the Knee Habilitation Project 
 *	Also contains auxiliary print functions 
*/

#include "preprocessing.h"
#include "txt2mat.h"
#include "quaternions.h"
#include "vector.h"
#include "resampling.h"
#include "horizontal_alignment.h"
#include "temporal_intervals_module.h"
#include <string.h>
#include <ctype.h>

#define RIGHT_L 1           // Right Leg Specifier
#define LEFT_L 0            // Left Leg Specifier 

typedef struct info {
	char first[100];
	char second[100];
	char dob[100];
	char sex;
	char date[100];
	char test[10];
	char months[100];
	int trial;
	int record;
	int segment;
	int height;
	int weight;
	int right[2];
	int left[2];
}Info;


/*
 *	Function: print_euler
 *	---------------------
 *	Auxiliary function used to print the quaternion angles to
 *	a .txt file
 * 
 *	euler: Pointer to the quaternion angle data vector
 *	num: Identification number to be used at the end of the 
 *      .txt filename
 *
 *	Returns: void
 */
void print_euler(vector *, int);
void print_euler(vector * euler, int num) {
	errno_t err;
	int i = 0;
	FILE *file;

	char buff[100];
	sprintf_s(buff, sizeof(buff), "output_euler_angles_%d.txt", num);
	err = fopen_s(&file, buff, "w");
	if (err != 0)
	{
		printf("There was a problem opening the files\n");
		return;
	}

	for (i = 0; i < euler->total; i++) {
		double *temp = VECTOR_GET(*euler, double *, i);
		fprintf(file, "%f\t", temp[0]);
		fprintf(file, "%f\t", temp[1]);
		fprintf(file, "%f\t", temp[2]);
		fprintf(file, "%f\t\n", temp[3]);
	}

	fclose(file);
}

/*	Function: print_euler_final 
*	--------------------------
*	used to print the various knee angles
*
*	t: Vector containing thigh angles
*	s: Vector containing shank angles
*/
void print_euler_final(vector *t, vector *s, int num) {

	errno_t err;
	int i = 0;
	FILE *file;

	double FE = 0;
	double VV = 0;
	double IE = 0;

	char buff[100];
	sprintf_s(buff, sizeof(buff), "output_euler_final_%d.txt", num);
	err = fopen_s(&file, buff, "w");
	if (err != 0)
	{
		printf("There was a problem opening the files\n");
		return;
	}

	fprintf(file, "Time\t");
	fprintf(file, "FE Angle Knee Joint\t");
	fprintf(file, "VV Angle Knee Joint\t");
	fprintf(file, "IE Angle Knee Joint\t");
	fprintf(file, "FE Angle Thigh\t");
	fprintf(file, "FE Angle Shank\n");

	for (i = 0; i < t->total; i++) {
		double *temp_t = VECTOR_GET(*t, double *, i);
		double *temp_s = VECTOR_GET(*s, double *, i);
		FE = temp_t[0] - temp_s[0];
		VV = temp_t[1] - temp_s[1];
		IE = temp_t[2] - temp_s[2];

		fprintf(file, "%f\t", temp_t[3]);
		fprintf(file, "%f\t", FE);
		fprintf(file, "%f\t", VV);
		fprintf(file, "%f\t", IE);
		fprintf(file, "%f\t", temp_t[0]);
		fprintf(file, "%f\t\n", temp_s[0]);
	}
	fclose(file);
}



/*
*	Function: print_temporal
*	---------------------
*	Auxiliary function used to print the gait parameter data to
*	a .txt file
*
*	vec: Pointer to the gait parameter data vector
*	type: 0 for Instants data, 1 for Intervals data
*	num: Identification number to be used at the end of the
*       .txt filename
*
*	Returns: void
*/
void print_temporal(vector *, int, int); 
void print_temporal(vector *vec, int type, int num) {

	errno_t err;
	int i = 0;
	FILE *file;
	int total = 0;

	char buff[100];
	if (type == 0) { // Instants 
		sprintf_s(buff, sizeof(buff), "output_instants_%d.txt", num);
		err = fopen_s(&file, buff, "w");
		total = 6;
	}
	else if (type == 1) {
		sprintf_s(buff, sizeof(buff), "output_intervals_%d.txt", num);
		err = fopen_s(&file, buff, "w");
		if (err != 0)
		{
			printf("There was a problem opening the files\n");
			return;
		}
		total = 3;
	}
	else
		return;

	for (i = 0; i < vec->total; i++) {
		double *temp = VECTOR_GET(*vec, double *, i);
		int j = 0;
		for (j = 0; j < total; j++)
			fprintf(file, "%f\t", temp[j]);
		fprintf(file, "\n");
	}
	fclose(file);
}


/*
 *	Function: menu
 *	---------------
 *	Displays a menu on the command line for the user
 *	to input data
 *
 *	i: A struct holding the patients inputted information
*/
void menu(Info *);
void menu(Info *i) {
	int done = 1;
	int cont = 1;
	char answer[10]; 
	while (done == 1) {
		system("cls");
		printf("******************************************\n\n\n");
		printf("             KNEE HABILITATION            \n\n\n");
		printf("******************************************\n\n");
		printf("Please Enter The Following Information,\n");
		printf("\n============ PERSONAL DETAILS ============\n\n");
		printf("\nFirst Name :\n");
		fgets(i->first, sizeof(i->first), stdin);
		printf("\nSurname :\n");
		fgets(i->second, sizeof(i->second), stdin);
		do {
			printf("\nDate Of Birth (dd/mm/yyyy) :\n");
			fgets(i->dob, sizeof(i->dob), stdin);
			if (i->dob[2] != '/' || i->dob[5] != '/') {
				printf("\nData Format Not Valid\n");
				cont = 0;
			}
			else
				cont = 1;
		} while (cont == 0);
		do {
			printf("\nSex (M/F) :\n");
			scanf_s("%c", &i->sex, 1);
			getchar();
			if (toupper(i->sex) == 'M' || toupper(i->sex) == 'F') {
				cont = 1;
			}
			else {
				printf("\nSex Not Valid\n");
				cont = 0;
			}
		} while (cont == 0);
		do {
			printf("\nHeight (cm) :\n");
			scanf_s("%d", &i->height);
			getchar();
			if (i->height < 0 || i->height > 300) {
				printf("\nHeigh Not Valid\n");
				cont = 0;
			}
			else
				cont = 1;
		} while (cont == 0);
		do {
			printf("\nWeight (kg) :\n");
			scanf_s("%d", &i->weight);
			getchar();
			if (i->weight < 0 || i->weight > 1000) {
				printf("\nWeight Not Valid\n");
				cont = 0;
			}
			else
				cont = 1;
		} while (cont == 0);
		do {
			printf("\nMonths To Or From Surgery, (use + if after surgery,\n- if before surgery) :\n");
			fgets(i->months, sizeof(i->months), stdin);
			if (i->months[0] == '-' || i->months[0] == '+') {
				cont = 1;
			}
			else {
				printf("\nInput Not Valid\n");
				cont = 0;
			}
		} while (cont == 0);
		printf("\n============ TEST DETAILS ================\n\n");
		do {
			printf("\nCurrent Data (dd/mm/yyyy) :\n");
			fgets(i->date, sizeof(i->date), stdin);
			if (i->dob[2] != '/' || i->dob[5] != '/') {
				printf("\nData Format Not Valid\n");
				cont = 0;
			}
			else
				cont = 1;
		} while (cont == 0);
		printf("\nType Of Test\n(e.g. W for walking, S for squat) :\n");
		fgets(i->test, sizeof(i->test), stdin);
		if (toupper(i->test[0]) == 'W') {
			do {
				printf("\nSegment Length (cm) :\n");
				scanf_s("%d", &i->segment);
				getchar();
				if ((((double)i->segment / (double)i->height ) < 0.1) || (((double)i->segment / (double)i->height) > 0.25)) {
					printf("\nInput Does Not Respect Human Proportions, Check Height And Segment Inputs...\n");
					do {
						printf("\nHeight (cm) :\n");
						scanf_s("%d", &i->height);
						getchar();
						if (i->height < 0 || i->height > 300) {
							printf("\nHeigh Not Valid\n");
							cont = 0;
						}
						else
							cont = 1;
					} while (cont == 0);
					cont = 0;
				}
				else
					cont = 1;
			} while (cont == 0);
		}
		printf("\nTrial Number :\n");
		scanf_s("%d", &i->trial);
		getchar();
		do {
			printf("\nRecord Number :\n");
			scanf_s("%d", &i->record);
			getchar();
			if (i->record < 0) {
				printf("Record Number Not Valid");
				cont = 0;
			}
			else
				cont = 1;
		} while (cont == 0);
		printf("\n============ IMU INFO ====================\n\n");
		printf("\nRight Thigh IMU Number :\n");
		scanf_s("%d", &i->right[0]);
		getchar();
		printf("\nRight Shank IMU Number :\n");
		scanf_s("%d", &i->right[1]);
		getchar();
		printf("\nLeft Thigh IMU Number :\n");
		scanf_s("%d", &i->left[0]);
		getchar();
		printf("\nLeft Shank IMU Number :\n");
		scanf_s("%d", &i->left[1]);
		getchar();
		system("cls");
		printf("\n========== You Entered ==================\n\n");
		printf("\nFirst Name :\t\t\t%s\n", i->first);
		printf("\nSurname :\t\t\t%s\n", i->second);
		printf("\nDate Of Birth :\t\t\t%s\n", i->dob);
		printf("\nSex :\t\t\t\t%c\n", i->sex);
		printf("\nHeight :\t\t\t%d\n", i->height);
		printf("\nWeight :\t\t\t%d\n", i->weight);
		printf("\nMonths To Or From Surgery :\t%s\n", i->months);
		printf("\nCurrent Date :\t\t\t%s\n", i->date);
		printf("\nType Of Test :\t\t\t%s\n", i->test);
		printf("\nTrial Number :\t\t\t%d\n", i->trial);
		printf("\nRecord Number :\t\t\t%d\n", i->record);
		if(toupper(i->test[0]) == 'W')
			printf("\nSegment Length :\t\t%d\n", i->segment);
		printf("\nRight Thigh IMU Number :\t%d\n", i->right[0]);
		printf("\nRight Shank IMU Number :\t%d\n", i->right[1]);
		printf("\nLeft Thigh IMU Number :\t\t%d\n", i->left[0]);
		printf("\nLeft Shank IMU Number :\t\t%d\n", i->left[1]);
		printf("\nIs The Preceeding Information Correct?\n");
		printf("Enter 'y' if correct or 'n' to restart :\n");
		fgets(answer, sizeof(answer), stdin);
		if(strcmp(answer, "y\n") == 0)
			done = 0;
	}
	system("cls");
	printf("\n=========== Program Running ==============\n\n");
}

/*	Function prev_main 
 *	-----------------
 *	for use with the older boards that the original
 *	matlab code was based on. I have left it here for 
 *	legacy purposes just in case there is a reversion 
 *	back to the original boards. 
*/
void prev_main() {

	//Info in;
	//menu(&in);
	int w = 0;                     // w == 0 for gait computation

	int i = 0;                 // Loop Counter
	int j = 0;                 // Loop Counter

	double Accf_rt[ACC_LENGTH];
	double Accf_rs[ACC_LENGTH];
	double Accf_lt[ACC_LENGTH];
	double Accf_ls[ACC_LENGTH];

	/* Create and set WIMU_Files structure
	Will contain data from each WIMU */
	WIMU_Files main_file;
	main_file.WIMU_rt = 4; // Right Thigh
	main_file.WIMU_rs = 18; // Right Shank
	main_file.WIMU_lt = 5; // Left Thigh
	main_file.WIMU_ls = 19; // Left Shank

	/* Initilize File Structs */
	Prev_Files first_files;
	Prev_Files second_files;
	Prev_Files third_files;
	Prev_Files fourth_files;

	/* WIMU 1 #4
	* Right Thigh
	*/ 
	open_prev_files(&first_files, main_file.WIMU_rt);
	VECTOR_INIT(first_WIMU_acc, first_files.num_lines);        // Create storage vector for the acc data
	VECTOR_INIT(first_WIMU_gyr, first_files.num_lines);        // Create storage vector for the gyr data
	first_files.acc_vec = &first_WIMU_acc;
	first_files.gyr_vec = &first_WIMU_gyr;
	scan_prev_files(&first_files, RIGHT_L);                    // Scan and import the data into storage
	VECTOR_INIT(first_cut_acc, record_size(&first_files, 13));  // Create storage vector for the cut acc data
	VECTOR_INIT(first_cut_gyr, record_size(&first_files, 13));  // Create storage vector for the cut gyr data 
	cut_record(&first_files, &first_cut_acc, &first_cut_gyr, 13);
	print_vectors(&first_files, 1);
	main_file.acc_rt = first_files.acc_vec;
	main_file.gyr_rt = first_files.gyr_vec;
	main_file.line_number_rt = first_files.num_lines;
	fclose(first_files.file_acc);
	fclose(first_files.file_gyr);
	fclose(first_files.file_ts);

	/* Filter Data */
	median_filter_set_up(main_file.acc_rt);       // Median Filter W/ Filter Size 20
	median_filter_set_up(main_file.gyr_rt);

	filter_vector(main_file.gyr_rt, FILTER_ORDER, CUTOFF);        // Low Pass Filter W/ Filter Size 2, Cuttoff 0.05/PI
	filter_vector(main_file.acc_rt, FILTER_ORDER, CUTOFF);

	 /* WIMU 2 #18
	  * Right Shank
	  */
	 open_prev_files(&second_files, main_file.WIMU_rs);
	 VECTOR_INIT(second_WIMU_acc, second_files.num_lines);        // Create storage vector for the acc data
	 VECTOR_INIT(second_WIMU_gyr, second_files.num_lines);        // Create storage vector for the gyr data
	 second_files.acc_vec = &second_WIMU_acc;
	 second_files.gyr_vec = &second_WIMU_gyr;
	 scan_prev_files(&second_files, RIGHT_L);                      // Scan and import the data into storage
	 VECTOR_INIT(second_cut_acc, record_size(&second_files, 13));  // Create storage vector for the cut acc data
	 VECTOR_INIT(second_cut_gyr, record_size(&second_files, 13));  // Create storage vector for the cut gyr data 
	 cut_record(&second_files, &second_cut_acc, &second_cut_gyr, 13);
	 print_vectors(&second_files, 2);
	 main_file.acc_rs = second_files.acc_vec;
	 main_file.gyr_rs = second_files.gyr_vec;
	 main_file.line_number_rs = second_files.num_lines;
	 fclose(second_files.file_acc);
	 fclose(second_files.file_gyr);
	 fclose(second_files.file_ts);

	 /* Filter Data */
	 median_filter_set_up(main_file.acc_rs);       // Median Filter W/ Filter Size 20
	 median_filter_set_up(main_file.gyr_rs);

	 filter_vector(main_file.gyr_rs, FILTER_ORDER, CUTOFF);        // Low Pass Filter W/ Filter Size 2, Cuttoff 0.05/PI
	 filter_vector(main_file.acc_rs, FILTER_ORDER, CUTOFF);

	  /* WIMU 3 #5
	   * Left Thigh
	   */
	 open_prev_files(&third_files, main_file.WIMU_lt);
	  VECTOR_INIT(third_WIMU_acc, third_files.num_lines);        // Create storage vector for the acc data
	  VECTOR_INIT(third_WIMU_gyr, third_files.num_lines);        // Create storage vector for the gyr data
	  third_files.acc_vec = &third_WIMU_acc;
	  third_files.gyr_vec = &third_WIMU_gyr;
	  scan_prev_files(&third_files, LEFT_L);                      // Scan and import the data into storage
	  VECTOR_INIT(third_cut_acc, record_size(&third_files, 13));  // Create storage vector for the cut acc data
	  VECTOR_INIT(third_cut_gyr, record_size(&third_files, 13));  // Create storage vector for the cut gyr data 
	  cut_record(&third_files, &third_cut_acc, &third_cut_gyr, 13);
	  print_vectors(&third_files, 3);
	  main_file.acc_lt = third_files.acc_vec;
	  main_file.gyr_lt = third_files.gyr_vec;
	  main_file.line_number_lt = third_files.num_lines;
	  fclose(third_files.file_acc);
	  fclose(third_files.file_gyr);
	  fclose(third_files.file_ts);

	  /* Filter Data */
	  median_filter_set_up(main_file.acc_lt);       // Median Filter W/ Filter Size 20
	  median_filter_set_up(main_file.gyr_lt);

	  filter_vector(main_file.gyr_lt, FILTER_ORDER, CUTOFF);        // Low Pass Filter W/ Filter Size 2, Cuttoff 0.05/PI
	  filter_vector(main_file.acc_lt, FILTER_ORDER, CUTOFF);

	 /* WIMU 4 #19
	  * Left Shank 
	  */
	  open_prev_files(&fourth_files, main_file.WIMU_ls);
	  VECTOR_INIT(fourth_WIMU_acc, fourth_files.num_lines);        // Create storage vector for the acc data
	  VECTOR_INIT(fourth_WIMU_gyr, fourth_files.num_lines);        // Create storage vector for the gyr data
	  fourth_files.acc_vec = &fourth_WIMU_acc;
	  fourth_files.gyr_vec = &fourth_WIMU_gyr;
	  scan_prev_files(&fourth_files, LEFT_L);                       // Scan and import the data into storage
	  VECTOR_INIT(fourth_cut_acc, record_size(&fourth_files, 13));  // Create storage vector for the cut acc data
	  VECTOR_INIT(fourth_cut_gyr, record_size(&fourth_files, 13));  // Create storage vector for the cut gyr data 
	  cut_record(&fourth_files, &fourth_cut_acc, &fourth_cut_gyr, 13);
	  print_vectors(&fourth_files, 4);
	  main_file.acc_ls = fourth_files.acc_vec;
	  main_file.gyr_ls = fourth_files.gyr_vec;
	  main_file.line_number_ls = fourth_files.num_lines;
	  fclose(fourth_files.file_acc);
	  fclose(fourth_files.file_gyr);
	  fclose(fourth_files.file_ts);

	  /* Filter Data */
	  median_filter_set_up(main_file.acc_ls);       // Median Filter W/ Filter Size 20
	  median_filter_set_up(main_file.gyr_ls);

	  filter_vector(main_file.gyr_ls, FILTER_ORDER, CUTOFF);        // Low Pass Filter W/ Filter Size 2, Cuttoff 0.05/PI
	  filter_vector(main_file.acc_ls, FILTER_ORDER, CUTOFF);
	

	/* Quaternion Calculation WIMU 1 */

	int line_number_rt = main_file.line_number_rt;
	VECTOR_INIT(euler_angles_rt, line_number_rt);        // Create storage vector for quaternion calculations
	set_up_filters(main_file.acc_rt, Accf_rt);
	quaternion_calc(main_file.acc_rt, main_file.gyr_rt, &euler_angles_rt, Accf_rt);

	/* Quaternion Calculation WIMU 2 */

	int line_number_rs = main_file.line_number_rs;
	VECTOR_INIT(euler_angles_rs, line_number_rs);        // Create storage vector for quaternion calculations
	set_up_filters(main_file.acc_rs, Accf_rs);
	quaternion_calc(main_file.acc_rs, main_file.gyr_rs,&euler_angles_rs, Accf_rs);

	/* Quaternion Calculation WIMU 3 */

	 int line_number_lt= main_file.line_number_lt;
	 VECTOR_INIT(euler_angles_lt, line_number_lt);        // Create storage vector for quaternion calculations
	 set_up_filters(main_file.acc_lt, Accf_lt);
	 quaternion_calc(main_file.acc_lt, main_file.gyr_lt,&euler_angles_lt, Accf_lt);

	/* Quaternion Calculation WIMU 4 */

	 int line_number_ls = main_file.line_number_ls;
	 VECTOR_INIT(euler_angles_ls, line_number_ls);        // Create storage vector for quaternion calculations
	 set_up_filters(main_file.acc_ls, Accf_ls);
	 quaternion_calc(main_file.acc_ls, main_file.gyr_ls,&euler_angles_ls, Accf_ls);

	 printf("\n==== Quaternion Calculation Complete ====\n");

	/* Linear Interpolation 1 */
	lininterp1(&euler_angles_rt); // Right Thigh 
	lininterp1(&euler_angles_rs); // Right Shank
	lininterp1(&euler_angles_lt); // Left Thigh
	lininterp1(&euler_angles_ls); // Left Shank

	/* Linear Interpolation 2 */

	lininterp2(&euler_angles_rt); // Right Thigh
	lininterp2(&euler_angles_rs); // Right Shank
	lininterp2(&euler_angles_lt); // Left Thigh
	lininterp2(&euler_angles_ls); // Left Shank

								  /* Resampling */
	printf("\n=========Interpolation Complete ==========\n");

	vec v;
	v.eul_1 = &euler_angles_rt;            // Set Up Right Thigh
	VECTOR_INIT(tmp_1, v.eul_1->total);   // Create storage for temporary vector
	set_up_resample(v.eul_1, &tmp_1);
	v.eul_1_tmp = &tmp_1;

	v.eul_2 = &euler_angles_rs;            // Set Up Right Shank
	VECTOR_INIT(tmp_2, v.eul_2->total);   // Create storage for temporary vector
	set_up_resample(v.eul_2, &tmp_2);
	v.eul_2_tmp = &tmp_2;

	v.eul_3 = &euler_angles_lt;            // Set Up Left Thigh
	VECTOR_INIT(tmp_3, v.eul_3->total);   // Create storage for temporary vector
	set_up_resample(v.eul_3, &tmp_3);
	v.eul_3_tmp = &tmp_3;

	v.eul_4 = &euler_angles_ls;            // Set Up Left Shank
	VECTOR_INIT(tmp_4, v.eul_4->total);   // Create storage for temporary vector
	set_up_resample(v.eul_4, &tmp_4);
	v.eul_4_tmp = &tmp_4;

	v.min_right = (euler_angles_rt.total < euler_angles_rs.total) ? 0 : 1;   // Find Shorter Vector
	v.min_left = (euler_angles_lt.total < euler_angles_ls.total) ? 0 : 1;

	resampling_right(&v);           // Resample the Right Leg Data
	resampling_left(&v);            // Resample the Left Leg Data

	v.min_both = (euler_angles_rt.total < euler_angles_lt.total) ? 0 : 1;   // Find the Shorter vector

	print_euler(&euler_angles_rt, 404);
	print_euler(&euler_angles_rs, 1804);
	print_euler(&euler_angles_lt, 504);
	print_euler(&euler_angles_ls, 1904);

	resampling_both(&v);           // Resample Both Legs Data
	printf("\n========== Resampling Complete ===========\n");
	VECTOR_FREE(tmp_1);            // Free Temporary Storage
	VECTOR_FREE(tmp_2);
	VECTOR_FREE(tmp_3);
	VECTOR_FREE(tmp_4);

	/* Horizontal Rotation of Shank Euler Angles */

	double theta_r = 90 * PI / 180;
	double theta_l = 90 * PI / 180;

	horizontal(main_file.gyr_rt, main_file.gyr_rs, &euler_angles_rs, &theta_r, RIGHT_L); // Right Shank
	horizontal(main_file.gyr_lt, main_file.gyr_ls, &euler_angles_ls, &theta_l, LEFT_L); // Left Shank

	horizontal_rotation_gyr(main_file.gyr_rs, theta_r);
	horizontal_rotation_gyr(main_file.acc_rs, theta_r);
	horizontal_rotation_gyr(main_file.gyr_ls, theta_l);
	horizontal_rotation_gyr(main_file.acc_ls, theta_l);

	print_vectors(&first_files, 10);
	print_vectors(&second_files, 20);
	print_vectors(&third_files, 30);
	print_vectors(&fourth_files, 40);

	print_vectors(&first_files, 1);
	print_vectors(&second_files, 1);
	print_vectors(&third_files, 1);
	print_vectors(&fourth_files, 1);


	printf("\n====== Horizontal Rotation Complete =======\n");
	print_euler_final(&euler_angles_rt, &euler_angles_rs, 1);
	print_euler_final(&euler_angles_lt, &euler_angles_ls, 2);

	if (w == 0) {
		/* Compute the temporal instants */

		double freq_1 = sample_freq(&euler_angles_rs);        // Right Shank
		double freq_2 = sample_freq(&euler_angles_ls);        // Left Shank

		int start = 500;
		int num_peaks_1 = 0;
		int num_peaks_2 = 0;
		int *peaks_1 = get_peaks(main_file.gyr_rs, start, &num_peaks_1);
		int *peaks_2 = get_peaks(main_file.gyr_ls, start, &num_peaks_2);

		VECTOR_INIT(instants_1, num_peaks_1);
		VECTOR_INIT(intervals_1, num_peaks_1);
		VECTOR_INIT(stridelength_1, num_peaks_1);

		VECTOR_INIT(instants_2, num_peaks_2);
		VECTOR_INIT(intervals_2, num_peaks_2);
		VECTOR_INIT(stridelength_2, num_peaks_2);

		VECTOR_INIT(ds, num_peaks_2);


		temporal_instants_1(&euler_angles_rs, main_file.gyr_rs, peaks_1, &instants_1, num_peaks_1);
		temporal_instants_1(&euler_angles_ls, main_file.gyr_ls, peaks_2, &instants_2, num_peaks_2);
		free(peaks_1);
		free(peaks_2);

		print_temporal(&instants_1, 0, 1);
		print_temporal(&instants_2, 0, 2);

		single_leg_temporal_intervals(&intervals_1, &instants_1, main_file.gyr_rs);
		single_leg_temporal_intervals(&intervals_2, &instants_2, main_file.gyr_ls);

		print_temporal(&intervals_1, 1, 1);
		print_temporal(&intervals_2, 1, 2);

		int segment = 20;                // Arbitrary segment length

		combined_temporal_intervals(&instants_1, &instants_2, &ds, main_file.gyr_rs);

		compute_stridelength(&stridelength_1, &instants_1, main_file.acc_rs, main_file.gyr_rs, (int)freq_1, segment);
		compute_stridelength(&stridelength_2, &instants_2, main_file.acc_ls, main_file.gyr_ls, (int)freq_2, segment);

		double mean_r = 0;
		double mean_l = 0;
		double sum_r = 0;
		double sum_l = 0;

		for (i = 0; i < intervals_1.total; i++) {
			double *tmp_rs = VECTOR_GET(intervals_1, double *, i);
			mean_r = mean_r + tmp_rs[0];
		}
		for (i = 0; i < intervals_2.total; i++) {
			double *tmp_ls = VECTOR_GET(intervals_2, double *, i);
			mean_l = mean_l + tmp_ls[0];
		}
		mean_r = mean_r / intervals_1.total;
		mean_l = mean_l / intervals_2.total;

		double cadence_r = 60 / mean_r;
		double cadence_l = 60 / mean_l;

		/*  Compute  variance  and standard deviation  */
		for (i = 0; i < intervals_1.total; i++)
		{
			double *tmp_rs = VECTOR_GET(intervals_1, double *, i);
			sum_r = sum_r + pow((tmp_rs[0] - mean_r), 2);
		}
		for (i = 0; i < intervals_2.total; i++) {
			double *tmp_ls = VECTOR_GET(intervals_2, double *, i);
			sum_l = sum_l + pow((tmp_ls[0] - mean_l), 2);
		}
		double variance_r = sum_r / intervals_1.total;
		double variance_l = sum_l / intervals_2.total;
		double std_deviation_r = sqrt(variance_r);
		double std_deviation_l = sqrt(variance_l);

		double gate_v_r = std_deviation_r / mean_r;
		double gate_v_l = std_deviation_l / mean_l;

		VECTOR_INIT(sp_r, stridelength_1.total);
		VECTOR_INIT(sp_l, stridelength_2.total);

		for (i = 0; i < stridelength_1.total; i++) {
			double *tmp_sl = VECTOR_GET(stridelength_1, double *, i);
			double *tmp_gct = VECTOR_GET(intervals_1, double *, (int)tmp_sl[1]);
			double *sp_row = malloc(sizeof(double));
			sp_row[0] = tmp_sl[0] / tmp_gct[0];
			VECTOR_ADD(sp_r, sp_row);
		}

		for (i = 0; i < stridelength_2.total; i++) {
			double *tmp_sl = VECTOR_GET(stridelength_2, double *, i);
			double *tmp_gct = VECTOR_GET(intervals_2, double *, (int)tmp_sl[1]);
			double *sp_row = malloc(sizeof(double));
			sp_row[0] = tmp_sl[0] / tmp_gct[0];
			VECTOR_ADD(sp_l, sp_row);
		}

		gait g;                              // Store gait vectors in gait structure
		g.intervals_r = &intervals_1;
		g.intervals_l = &intervals_2;
		g.stridelength_r = &stridelength_1;
		g.stridelength_l = &stridelength_2;
		g.sp_r = &sp_r;
		g.sp_l = &sp_l;
		g.cadence_r = cadence_r;
		g.cadence_l = cadence_l;
		g.gate_v_r = gate_v_r;
		g.gate_v_l = gate_v_l;

		print_gait(&g);                      // Print the gait parameters

		VECTOR_FREE(instants_1);
		VECTOR_FREE(instants_2);
		VECTOR_FREE(intervals_1);
		VECTOR_FREE(intervals_2);
		VECTOR_FREE(stridelength_1);
		VECTOR_FREE(stridelength_2);
		VECTOR_FREE(ds);
		VECTOR_FREE(sp_r);
		VECTOR_FREE(sp_l);
	}

	VECTOR_FREE(*main_file.gyr_rt);      // Free Remaining Storage Vectors
	VECTOR_FREE(*main_file.gyr_rs);
	VECTOR_FREE(*main_file.gyr_lt);
	VECTOR_FREE(*main_file.gyr_ls);
	VECTOR_FREE(*main_file.acc_rt);
	VECTOR_FREE(*main_file.acc_rs);
	VECTOR_FREE(*main_file.acc_lt);
	VECTOR_FREE(*main_file.acc_ls);

	printf("\n============ Program Complete =============\n");
	return;         // Program Completed Sucessfully
}


/*
*	Function: main
*	---------------------
*	main function that is invoked when the program is run.
*	includes all necessary function calls for the program
*
*	Returns: 0 if the program is completed sucessfully, 1
*            a problem was encountered
*/
int main(void) {

	/*prev_main();
	return;*/

	double Accf_rt[ACC_LENGTH];
	double Accf_rs[ACC_LENGTH];
	double Accf_lt[ACC_LENGTH];
	double Accf_ls[ACC_LENGTH];

	int i = 0;                 // Loop Counter
	int j = 0;                 // Loop Counter

	/* Create and populate menu */
	Info in;
	menu(&in);

	/* Initialize file structs */
	WIMU_Files vectors;
	Sens_Files sens_files;
	sens_files.all_files = &vectors;
	sens_files.record_number = in.record;
	sens_files.euler = NO_EUL;
	sens_files.sensor_nums_r = in.right;
	sens_files.sensor_nums_l = in.left;

	open_sens_files(&sens_files);                         // Open files

	/* Initialize acc, gyr, and euler vectors */
	VECTOR_INIT(acc_rt, vectors.line_number_rt);
	VECTOR_INIT(gyr_rt, vectors.line_number_rt);
	VECTOR_INIT(acc_rs, vectors.line_number_rs);
	VECTOR_INIT(gyr_rs, vectors.line_number_rs);
	VECTOR_INIT(acc_lt, vectors.line_number_lt);
	VECTOR_INIT(gyr_lt, vectors.line_number_lt);
	VECTOR_INIT(acc_ls, vectors.line_number_ls);
	VECTOR_INIT(gyr_ls, vectors.line_number_ls);
	VECTOR_INIT(euler_angles_rt, vectors.line_number_rt);
	VECTOR_INIT(euler_angles_rs, vectors.line_number_rs);
	VECTOR_INIT(euler_angles_lt, vectors.line_number_lt);
	VECTOR_INIT(euler_angles_ls, vectors.line_number_ls);
	vectors.acc_rt = &acc_rt;
	vectors.gyr_rt = &gyr_rt;
	vectors.acc_rs = &acc_rs;
	vectors.gyr_rs = &gyr_rs;
	vectors.acc_lt = &acc_lt;
	vectors.gyr_lt = &gyr_lt;
	vectors.acc_ls = &acc_ls;
	vectors.gyr_ls = &gyr_ls;
	vectors.eul_rt = &euler_angles_rt;
	vectors.eul_rs = &euler_angles_rs;
	vectors.eul_lt = &euler_angles_lt;
	vectors.eul_ls = &euler_angles_ls;

	/* Scan files and populate vectors */
	scan_sens_files(&sens_files);

	/* Median filter with filter size 20 */
	median_filter_set_up(&acc_rt);
	median_filter_set_up(&acc_rs);
	median_filter_set_up(&acc_lt);
	median_filter_set_up(&acc_ls);
	median_filter_set_up(&gyr_rt);
	median_filter_set_up(&gyr_rs);
	median_filter_set_up(&gyr_lt);
	median_filter_set_up(&gyr_ls);

	/* Low pass filter w/ filter size 2, cuttoff 0.05/pi */
	filter_vector(&acc_rt, FILTER_ORDER, CUTOFF);
	filter_vector(&acc_rs, FILTER_ORDER, CUTOFF);
	filter_vector(&acc_lt, FILTER_ORDER, CUTOFF);
	filter_vector(&acc_ls, FILTER_ORDER, CUTOFF);
	filter_vector(&gyr_rt, FILTER_ORDER, CUTOFF);
	filter_vector(&gyr_rs, FILTER_ORDER, CUTOFF);
	filter_vector(&gyr_lt, FILTER_ORDER, CUTOFF);
	filter_vector(&gyr_ls, FILTER_ORDER, CUTOFF);

	if (NO_EUL == 1) {
		/* Set up the quaternion calculations filters */
		set_up_filters(&acc_rt, Accf_rt);
		set_up_filters(&acc_rs, Accf_rs);
		set_up_filters(&acc_lt, Accf_lt);
		set_up_filters(&acc_ls, Accf_ls);

		/* Perform the quaternion calculations */
		quaternion_calc(&acc_rt, &gyr_rt, &euler_angles_rt, Accf_rt);
		quaternion_calc(&acc_rs, &gyr_rs, &euler_angles_rs, Accf_rs);
		quaternion_calc(&acc_lt, &gyr_lt, &euler_angles_lt, Accf_lt);
		quaternion_calc(&acc_ls, &gyr_ls, &euler_angles_ls, Accf_ls);
	}
	/* Linear Interpolation 1 */
	lininterp1(&euler_angles_rt); // Right Thigh 
	lininterp1(&euler_angles_rs); // Right Shank
	lininterp1(&euler_angles_lt); // Left Thigh
	lininterp1(&euler_angles_ls); // Left Shank

	/* Linear Interpolation 2 */

	lininterp2(&euler_angles_rt); // Right Thigh
	lininterp2(&euler_angles_rs); // Right Shank
	lininterp2(&euler_angles_lt); // Left Thigh
	lininterp2(&euler_angles_ls); // Left Shank

	/* Resampling */

	vec v;                                // Holds quaternion calculations for resmapling

	v.eul_1 = &euler_angles_rt;           // Set Up Right Thigh
	VECTOR_INIT(tmp_1, v.eul_1->total);   // Create storage for temporary vector
	set_up_resample(v.eul_1, &tmp_1);
	v.eul_1_tmp = &tmp_1;

	v.eul_2 = &euler_angles_rs;            // Set Up Right Shank
	VECTOR_INIT(tmp_2, v.eul_2->total);   // Create storage for temporary vector
	set_up_resample(v.eul_2, &tmp_2);
	v.eul_2_tmp = &tmp_2;

	v.eul_3 = &euler_angles_lt;            // Set Up Left Thigh
	VECTOR_INIT(tmp_3, v.eul_3->total);   // Create storage for temporary vector
	set_up_resample(v.eul_3, &tmp_3);
	v.eul_3_tmp = &tmp_3;

	v.eul_4 = &euler_angles_ls;            // Set Up Left Shank
	VECTOR_INIT(tmp_4, v.eul_4->total);   // Create storage for temporary vector
	set_up_resample(v.eul_4, &tmp_4);
	v.eul_4_tmp = &tmp_4;

	v.min_right = (euler_angles_rt.total < euler_angles_rs.total) ? 0 : 1;   // Find Shorter Vector
	v.min_left =  (euler_angles_lt.total < euler_angles_ls.total) ? 0 : 1;

	resampling_right(&v);           // Resample the Right Leg Data
	resampling_left(&v);            // Resample the Left Leg Data
	 
	v.min_both = (euler_angles_rt.total < euler_angles_lt.total) ? 0 : 1;   // Find the Shorter vector

	resampling_both(&v);           // Resample Both Legs Data

	VECTOR_FREE(tmp_1);            // Free Temporary Storage
	VECTOR_FREE(tmp_2);
	VECTOR_FREE(tmp_3);
	VECTOR_FREE(tmp_4);

	/* Horizontal Rotation of Shank Euler Angles */
	double theta_r = 90 * PI / 180;
	double theta_l = 90 * PI / 180;
	horizontal(&gyr_rt, &gyr_rs, &euler_angles_rs, &theta_r, RIGHT_L); // Right Shank
	horizontal(&gyr_lt, &gyr_ls, &euler_angles_ls, &theta_l, LEFT_L); // Left Shank

	horizontal_rotation_gyr(&gyr_rs, theta_r);
	horizontal_rotation_gyr(&acc_rs, theta_r);
	horizontal_rotation_gyr(&gyr_ls, theta_l);
	horizontal_rotation_gyr(&acc_ls, theta_l);

	/* Used for testing purposes */
	print_euler(&euler_angles_rt, 1);
	print_euler(&euler_angles_rs, 2);
	print_euler(&euler_angles_lt, 3);
	print_euler(&euler_angles_ls, 4);

	/* Print the knee joint angles */
	print_euler_final(&euler_angles_rt, &euler_angles_rs, 1);
	print_euler_final(&euler_angles_lt, &euler_angles_ls, 2);

	/* Compute Gait Parameters Walking Was Selected As The Test Type */
	if (toupper(in.test[0]) == 'W') {

		int num_peaks_rs = 0;                               // Number of peaks in right shank
		int num_peaks_ls = 0;                               // Number of peaks in left shank
		int small_peak = 0;                                 // Smallest number of peaks

		/* Compute the sampling frequency */
		double freq_rs = sample_freq(&euler_angles_ls);        // Left Shank
		double freq_ls = sample_freq(&euler_angles_rs);        // Left Shank

		/* Compute the number of peaks in the gyroscope data */
		int *peaks_rs = get_peaks(&gyr_rs, (int)(2 * freq_rs), &num_peaks_rs);
		int *peaks_ls = get_peaks(&gyr_ls, (int)(2 * freq_rs), &num_peaks_ls);

		if (num_peaks_rs == 0 || num_peaks_ls == 0) {
			printf("NO STRIDE FOUND");                           // No stride is found
		}
		else {

			/* Initialize Gait Vectors */
			VECTOR_INIT(instants_rs, num_peaks_rs);
			VECTOR_INIT(intervals_rs, num_peaks_rs);
			VECTOR_INIT(stridelength_rs, num_peaks_rs);

			VECTOR_INIT(instants_ls, num_peaks_ls);
			VECTOR_INIT(intervals_ls, num_peaks_ls);
			VECTOR_INIT(stridelength_ls, num_peaks_ls);

			if (num_peaks_rs < num_peaks_ls)
				small_peak = num_peaks_rs;
			else
				small_peak = num_peaks_ls;
			VECTOR_INIT(ds, small_peak);

			/* Temporal Instants */
			temporal_instants_1(&euler_angles_rs, &gyr_rs, peaks_rs, &instants_rs, num_peaks_rs);
			temporal_instants_1(&euler_angles_ls, &gyr_ls, peaks_ls, &instants_ls, num_peaks_ls);

			free(peaks_rs);                          // Free peaks_rs array
			free(peaks_ls);                          // Free peaks_ls array

			print_temporal(&instants_rs, 0, 1);
			print_temporal(&instants_ls, 0, 2);

			/* Temporal Intervals */
			single_leg_temporal_intervals(&intervals_rs, &instants_rs, &gyr_rs);
			single_leg_temporal_intervals(&intervals_ls, &instants_ls, &gyr_ls);

			print_temporal(&intervals_rs, 1, 1);
			print_temporal(&intervals_ls, 1, 2);

			/* Combined Temporal Instants */
			combined_temporal_intervals(&instants_rs, &instants_ls, &ds, &gyr_rs);

			/* Stridelength */
			compute_stridelength(&stridelength_rs, &instants_rs, &acc_rs, &gyr_rs, freq_rs, in.segment);
			compute_stridelength(&stridelength_ls, &instants_ls, &acc_ls, &gyr_ls, freq_ls, in.segment);

			double mean_r = 0;
			double mean_l = 0;
			double sum_r = 0;
			double sum_l = 0;

			for (i = 0; i < intervals_rs.total; i++) {
				double *tmp_rs = VECTOR_GET(intervals_rs, double *, i);
				mean_r = mean_r + tmp_rs[0];
			}
			for (i = 0; i < intervals_ls.total; i++) {
				double *tmp_ls = VECTOR_GET(intervals_ls, double *, i);
				mean_l = mean_l + tmp_ls[0];
			}
			mean_r = mean_r / intervals_rs.total;
			mean_l = mean_l / intervals_ls.total;

			double cadence_r = 60 / mean_r;
			double cadence_l = 60 / mean_l;

			/*  Compute  variance  and standard deviation  */
			for (i = 0; i < intervals_rs.total; i++)
			{
				double *tmp_rs = VECTOR_GET(intervals_rs, double *, i);
				sum_r = sum_r + pow((tmp_rs[0] - mean_r), 2);
			}
			for (i = 0; i < intervals_ls.total; i++) {
				double *tmp_ls = VECTOR_GET(intervals_ls, double *, i);
				sum_l = sum_l + pow((tmp_ls[0] - mean_l), 2);
			}
			double variance_r = sum_r / intervals_rs.total;
			double variance_l = sum_l / intervals_rs.total;
			double std_deviation_r = sqrt(variance_r);
			double std_deviation_l = sqrt(variance_l);	

			double gate_v_r = std_deviation_r / mean_r;
			double gate_v_l = std_deviation_l / mean_l;

			VECTOR_INIT(sp_r, stridelength_rs.total);
			VECTOR_INIT(sp_l, stridelength_ls.total);

			for (i = 0; i < stridelength_rs.total; i++) {
				double *tmp_sl = VECTOR_GET(stridelength_rs, double *, i);
				double *tmp_gct = VECTOR_GET(intervals_rs, double *, tmp_sl[1]);
				double *sp_row = malloc(sizeof(double));
				sp_row[0] = tmp_sl[0] / tmp_gct[0];
				VECTOR_ADD(sp_r, sp_row);
			}

			for (i = 0; i < stridelength_ls.total; i++) {
				double *tmp_sl = VECTOR_GET(stridelength_ls, double *, i);
				double *tmp_gct = VECTOR_GET(intervals_ls, double *, tmp_sl[1]);
				double *sp_row = malloc(sizeof(double));
				sp_row[0] = tmp_sl[0] / tmp_gct[0];
				VECTOR_ADD(sp_l, sp_row);
			}

			gait g;                              // Store gait vectors in gait structure
			g.intervals_r = &intervals_rs;
			g.intervals_l = &intervals_ls;
			g.stridelength_r = &stridelength_rs;
			g.stridelength_l = &stridelength_ls;
			g.sp_r = &sp_r;
			g.sp_l = &sp_l;
			g.cadence_r = cadence_r;
			g.cadence_l = cadence_l;
			g.gate_v_r = gate_v_r;
			g.gate_v_l = gate_v_l;

			print_gait(&g);                      // Print the gait parameters

			VECTOR_FREE(instants_rs);
			VECTOR_FREE(instants_ls);
			VECTOR_FREE(intervals_rs);
			VECTOR_FREE(intervals_ls);
			VECTOR_FREE(stridelength_rs);
			VECTOR_FREE(stridelength_ls);
			VECTOR_FREE(ds);
			VECTOR_FREE(sp_r);
			VECTOR_FREE(sp_l);
		}
	}

	/* Free Remaining Vectors */
	VECTOR_FREE(gyr_rt);
	VECTOR_FREE(gyr_rs);
	VECTOR_FREE(gyr_lt);
	VECTOR_FREE(gyr_ls);
	VECTOR_FREE(acc_rt);
	VECTOR_FREE(acc_rs);
	VECTOR_FREE(acc_lt);
	VECTOR_FREE(acc_ls);
	VECTOR_FREE(euler_angles_rt);
	VECTOR_FREE(euler_angles_rs);
	VECTOR_FREE(euler_angles_lt);
	VECTOR_FREE(euler_angles_ls);

	printf("\n============ Program Complete =============\n");
return 0;         // Program Completed Sucessfully
}

