#include "object.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int n_objects;
object_t** objects;
extern int center_object;

#define OBJECTS_INITIAL_SIZE 32

static void add_object(object_t* object) {
    static int objects_size = 0;

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
}

void save_state(const char* path) {
    int i, fd;
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    for (i=0; i<n_objects; i++) {
        write(fd, objects[i], sizeof(object_t));
    }
    close(fd);
}

void load_state(const char* path) {
    int fd;
    size_t count;
    object_t* object;

    n_objects = 0;
    fd = open(path, O_RDONLY);

    while (true) {
        object = malloc(sizeof(object_t));
        count = read(fd, object, sizeof(object_t));
        if (count == sizeof(object_t)) {
            add_object(object);

        } else if (count == 0) {
            break;

        } else {
            fprintf(stderr, "Error: Tried to load from non-aligned file!\n");
            fprintf(stderr, "%dth object consisted of %ld (out of %ld) bytes.\n", n_objects, count, sizeof(object_t));
            fprintf(stderr, "Are you sure you loaded the right file?\n");
            fprintf(stderr, "Path: %s\n", path);
            break;
        }
    }
    free(object); // Last read is allways empty
    object = NULL;
}

