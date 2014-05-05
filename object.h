#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"

typedef struct object {
    f_vec_t p; /* Position */
    f_vec_t v; /* Velocity */
	double 	m;	/* Mass */
	double 	r; 	/* Radius */
} object_t;

static void add_object(object_t* object);
void insert_new_object(f_vec_t p, double m, double r, f_vec_t v);
void merge_objects(object_t* o1, object_t* o2);
void remove_object(int i);

#endif
