/*
*	vector.c
*	Provides implementation for vector.h header file
*/

#include "vector.h"

void vector_init_cap(vector *v) {

	v->capacity = VECTOR_INIT_CAPACITY + 100;
	v->total = 0;
	v->items = calloc(v->capacity, sizeof(double *));
	if (v->items == NULL) {
		printf("could'nt allocate memory A!");
		exit(EXIT_FAILURE);
	}

}

void vector_init(vector *v, int num) {

	v->capacity = num + 100;
	v->total = 0;
	v->items = calloc(v->capacity, sizeof(double *));
	if (v->items == NULL) {
		printf("could'nt allocate memory!, num: %d", num);
		exit(EXIT_FAILURE);
	}

}

void vector_init_int(vector *v, int num) {
	v->capacity = num;
	v->total = 0;
	v->items_int = calloc(v->capacity, sizeof(int *));
	if (v->items_int == NULL) {
		printf("could'nt allocate memory C!");
		exit(EXIT_FAILURE);
	}
}

int vector_total(vector *v) {

	return v->total;

}

static void vector_resize(vector *v, int capacity) {

#ifdef DEBUG_ON
	printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

	double **items = realloc(v->items, sizeof(double *) * capacity);
	if (items == NULL) {
		printf("could'nt allocate memory!");
		exit(EXIT_FAILURE);
	}
	if (items) {
		v->items = items;
		v->capacity = capacity;
	}

}

static void vector_resize_int(vector *v, int capacity) {

#ifdef DEBUG_ON
	printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

	int **items_int = realloc(v->items_int, sizeof(int*) * v->capacity);
	if (items_int == NULL) {
		printf("could'nt allocate memory!");
		exit(EXIT_FAILURE);
	}
	if (items_int) {
		v->items_int = items_int;
		v->capacity = capacity;
	}

}

void vector_add(vector *v, double *item)
{

	if (v->capacity == v->total) {
		vector_resize(v, v->capacity + 50);
	}
	v->items[v->total++] = item;

}

void vector_add_int(vector *v, int *item) {
	if (v->capacity == v->total)
		vector_resize(v, v->capacity + 100);
	v->items_int[v->total++] = item;
}

void vector_set(vector *v, int index, double *item)
{
	if (index >= 0 && index < v->total) {
		double *tmp = VECTOR_GET(*v, double*, index);
		tmp[0] = item[0];
		tmp[1] = item[1];
		tmp[2] = item[2];
		tmp[3] = item[3];
	}
	else {
		printf("There was a problem seting the vector!\n");
	}
}

/* inserts new element after the index given */
void vector_insert(vector *v, int index, double *insert) {

	if (index <= 0 || index >= v->total) {
		return;
	}
	if (v->capacity == v->total) {
		vector_resize(v, v->capacity + 1000);
	}
	int i = 0;
	double *tmp = (double*)calloc(4, sizeof(double));
	VECTOR_ADD(*v, tmp);
	for (i = (v->total - 1); i > (index + 1); i--) {
		double *tmp_row_1 = VECTOR_GET(*v, double*, i);
		double *tmp_row_2 = VECTOR_GET(*v, double*, i - 1);
		tmp_row_1[0] = tmp_row_2[0];
		tmp_row_1[1] = tmp_row_2[1];
		tmp_row_1[2] = tmp_row_2[2];
		tmp_row_1[3] = tmp_row_2[3];
	}
	vector_set(v, index + 1, insert);
}

/* deletes the rows from (start,end) */
void vector_delete_section(vector* v, int start, int end) {

	if (start <= 0 || end <= 0 || start >= v->total || end > v->total) {
		return;
	}
	int i = 0;
	for (i = start + 1; i < end; i++) {
		free(v->items[i]);
		v->items[i] = NULL;
	}

	v->total = v->total - (end - start);

	if (v->total > 0 && v->total == v->capacity / 4)
		vector_resize(v, v->capacity / 2);
}

double *vector_get(vector *v, int index)
{

	if (index >= 0 && index < v->total)
		return v->items[index];
	return NULL;

}

int vector_get_int(vector *v, int index) {
	if (index >= 0 && index < v->total)
		return *(v->items_int[index]);
	return 1;
}

void vector_delete(vector *v, int index)
{

	if (index < 0 || index >= v->total)
		return;

	free(v->items[index]);
	v->items[index] = NULL;

	if (index != v->total - 1) {
		int i = 0;
		for (i = index; i < v->total - 1; i++) {
			v->items[i] = v->items[i + 1];
			free(v->items[i + 1]);
			v->items[i + 1] = NULL;
		}
	}

	v->total--;

	if (v->total > 0 && v->total == v->capacity / 4)
		vector_resize(v, v->capacity / 2);

}

void vector_free(vector *v)
{
	//printf("FREE HAS OCCURED!\n");

	int i = 0;
	for (i = 0; i < v->total; i++) {
		free(v->items[i]);
	}
	free(v->items);
	v->items = NULL;
	v->total = 0;

}