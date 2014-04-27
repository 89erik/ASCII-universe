#include "object.h"

#include <stdio.h>
#include <stdlib.h>

int n_objects;
object_t** objects;

#define OBJECTS_INITIAL_SIZE 32

static void add_object(object_t* object) {
    static int objects_size = 0;
    object_t** tmp;

    if (!(n_objects+1 < objects_size)) {
        if (objects_size == 0) objects_size = OBJECTS_INITIAL_SIZE;
        else                   objects_size *= 2;

        objects = (object_t**) realloc(objects, sizeof(object_t*) * objects_size);
    }
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

