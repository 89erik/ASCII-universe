#include "object.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

int n_objects;
object_t** objects;
extern int center_object;

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

void insert_new_object(f_vec_t p, double m, double r, f_vec_t v) {
    object_t* object = (object_t*) malloc(sizeof(object_t));
    object->p = p;
    object->m = m;
    object->r = r;
    object->v = v;
    object->a.x = 0.0;
    object->a.y = 0.0;

    add_object(object);
}

void merge_objects(object_t* o1, object_t* o2) {
    double combined_m = o1->m + o2->m;
    double w1 = o1->m / combined_m;
    double w2 = o2->m / combined_m;

    if (o1->m < o2->m) {
        o1->p = o2->p;
    }

    o1->v.x = w1 * o1->v.x + w2 * o2->v.x;
    o1->v.y = w1 * o1->v.y + w2 * o2->v.y;
    

    o1->m = combined_m;
    o1->r = sqrt(pow(o1->r,2) + pow(o2->r,2));
}

void remove_object(int i) {
    free(objects[i]);
    if (center_object > i) {
        center_object--;
    }
    for (n_objects--; i<n_objects; i++) {
        objects[i] = objects[i+1];
    }

    printf("objects:\n");
    for (i=0; i<n_objects; i++) {
        printf("#%d, r = %f,\tm = %f\n", i, objects[i]->r, objects[i]->m);
    }

}


