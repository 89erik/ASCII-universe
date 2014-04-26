#include "object.h"

#include <stdio.h>
#include <stdlib.h>

int n_objects;
object_t** objects;

static void add_object(object_t* object) {
    object_t** tmp;
    size_t new_size = sizeof(object_t*) * (n_objects + 1);
    objects = (object_t**) realloc(objects, new_size);
    objects[n_objects++] = object;
}

void insert_new_object(double x, double y, double m, double r, double vx, double vy) {
    object_t* object = (object_t*) malloc(sizeof(object_t));
    object->x = x;
    object->y = y;
    object->m = m;
    object->r = r;
    object->vx = vx;
    object->vy = vy;
    object->ax = 0.0;
    object->ay = 0.0;

    add_object(object);
}

