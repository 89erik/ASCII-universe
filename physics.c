#include "gravity.h"
#include "physics.h"
#include "object.h"
#include "vector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


extern int n_objects;
extern object_t** objects;
extern int center_object;

/*
 * Calculates the distance between two objects
 */
double distance(object_t* o1, object_t* o2){
    return f_vec_euclidean_distance(o1->p, o2->p);
}

/*
 * Determines if o1 and o2 are intersecting
 */
bool intersects_with_factor(object_t* o1, object_t* o2, double factor) {
    double dist = distance(o1,o2);
    if (dist < (o1->r + o2->r) * factor) {
        return true;
    }
    return false;
}
/*
 * Determines if o1 and o2 are intersecting
 */
bool intersects(object_t* o1, object_t* o2) {
    return intersects_with_factor(o1,o2,1); 
}

#define MAX_MERGE_VELOCITY    100
#define MERGE_DISTANCE_FACTOR 0.5
bool merge(object_t* o1, object_t* o2) {
    f_vec_t vd; // Difference in speed
   
    if (!intersects_with_factor(o1,o2,MERGE_DISTANCE_FACTOR)) return false;

    vd = f_vec_diff(o1->v, o2->v);
    if (!(vd.x < MAX_MERGE_VELOCITY && vd.y < MAX_MERGE_VELOCITY)) return false;
    
    return true;
}

/*
 * Calculates the gravitational force between 
 * two objects and updates the first object's 
 * acceleration accordingly.
 */ 
void apply_grav_force(object_t* o1, object_t* o2, f_vec_t* acceleration) {
    if (o1->m == 0 || o2->m == 0) return;

    double dist = distance(o1,o2);
    double sqrd_dist = pow(dist, 2);
    double a = GRAV_CONST * o2->m / sqrd_dist;  /* a = GM/r² */
    
    f_vec_t pd = f_vec_diff(o1->p, o2->p);

    double theta;

    f_vec_t a_vec;

    if (o1->p.y < o2->p.y) {        /* o1 above */
        theta = atan(pd.x/pd.y);
        a_vec.x = a * sin(theta);
        a_vec.y = a * cos(theta);

    } else if (o1->p.y > o2->p.y) { /* o1 under */
        theta = atan(pd.y/pd.x);
        a_vec.x = a * cos(theta);
        a_vec.y = -a * sin(theta);
    } else if (o1->p.y == o2->p.y) {    /* Equal heights */
        a_vec.y = 0;
        a_vec.x = a;
    }
    if (o1->p.x > o2->p.x) {        /* o1 to the right*/
        a_vec.x = -a_vec.x;
    }

    f_vec_accumulate(acceleration, &a_vec);
}

/*
 * Performs the physics of one time quantum
 */
void tick(void) {
    int i,j;
    f_vec_t acceleration;
    for (i=0; i<n_objects; i++) {
        /* Determines gravity acceleration*/
        acceleration = (f_vec_t) {0.0, 0.0};
        for (j=0; j<n_objects; j++) {
            if (objects[i] == objects[j]) continue;
            if (intersects(objects[i], objects[j])) {
                if (merge(objects[i], objects[j])) {
                    merge_objects(objects[i], objects[j]);
                    if (center_object == j) center_object = i;
                    remove_object(j);
                    j--; 
                    continue;
                }
            }
            apply_grav_force(objects[i], objects[j], &acceleration);
        }
        /* Applies acceleration to speed */
        f_vec_accumulate(&objects[i]->v, &acceleration);

        /* Moves */
        f_vec_accumulate(&objects[i]->p, &objects[i]->v);

    }
}
