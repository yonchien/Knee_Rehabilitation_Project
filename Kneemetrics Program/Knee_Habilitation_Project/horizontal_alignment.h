/*
 *	Class: horizontal_alignment.h
 *	-----------------------------
 */


#ifndef HORIZONTAL_ALIGNMENT_H
#define HORIZONTAL_ALIGNMENT_H


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"
#include <math.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>


dgesvd_(const char* jobu, const char* jobvt, const int* M, const int* N,
	double* A, const int* lda, double* S, double* U, const int* ldu,
	double* VT, const int* ldvt, double* work, const int* lwork, const
	int* info);
void dscal_(int* n, double* alpha, double* x, int* incx);
void dgemm_(char* transa, char* transb, int* m, int* n, int* k,
	double* alpha, double* a, int* lda,
	double* b, int* ldb, double* beta,
	double* c, int* ldc);

#define PI 3.14159265
#define SAMPLE_N 1000
#define NMAX 50
#define RIGHT 1
#define LEFT 0

/* Parameters For SVD Test*/
#define M 1000
#define N 1
#define LDA M
#define LDU M
#define LDVT N
#define K LDVT

void horizontal(vector *, vector *, vector *, double *, int);

void horizontal_rotation(double, vector *);

double norm(double *);

void singular_value_decomposition(double *, double* );

#endif