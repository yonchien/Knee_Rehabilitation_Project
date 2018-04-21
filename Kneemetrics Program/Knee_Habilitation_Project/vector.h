/*
*	vector.h
*	Creates a dynamically allocated vector of double precision
*	pointers to be used for storing WIMU data from txt files
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4

/* Macros */
#define VECTOR_INIT(vec, id) vector vec; vector_init(&vec, id)
#define VECTOR_INIT_CAP(vec) vector vec; vector_init_cap(&vec)
#define VECTOR_INIT_INT(vec, id) vector vec; vector_init_int(&vec, id)
#define VECTOR_ADD(vec, item) vector_add(&vec, (double *)item)
#define VECTOR_SET(vec, id, item) vector_set(&vec, id, (double *) item)
#define VECTOR_GET(vec, type, id) (type) vector_get(&vec, id)
#define VECTOR_DELETE(vec, id) vector_delete(&vec, id)
#define VECTOR_TOTAL(vec) vector_total(&vec)
#define VECTOR_FREE(vec) vector_free(&vec)
#define VECTOR_INSERT(vec, id, in) vector_insert(&vec, id, in)
#define VECTOR_DELETE_SEC(vec, st, en) vector_delete_section(&vec, st, en)

typedef struct vector {
	double **items;
	int **items_int;
	int capacity;
	int total;
} vector;

void vector_init_cap(vector *);

void vector_init(vector *, int);

void vector_init_int(vector *, int);

int vector_total(vector *);

static void vector_resize(vector *, int);

static void vector_resize_int(vector *, int);

void vector_add(vector *, double *);

void vector_add_int(vector *, int *);

void vector_set(vector *, int, double *);

double *vector_get(vector *, int);

int vector_get_int(vector *, int);

void vector_delete(vector *, int);

void vector_free(vector *);

void vector_insert(vector *, int, double *);

void vector_delete_section(vector *, int, int);

#endif
