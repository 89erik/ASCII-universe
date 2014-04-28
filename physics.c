#include "gravity.h"
#include "physics.h"
#include "object.h"

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
	double dx = o2->x - o1->x;
	double dy = o2->y - o1->y;

	return sqrt(pow(dx, 2) + pow(dy, 2));
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
    double vxd, vyd; // Difference in speed
   
    if (!intersects_with_factor(o1,o2,MERGE_DISTANCE_FACTOR)) return false;

    vxd = abs(o1->vx - o2->vx);
    vyd = abs(o1->vy - o2->vy);
    if (!(vxd < MAX_MERGE_VELOCITY && vyd < MAX_MERGE_VELOCITY)) return false;
    
    return true;
}

/*
 * Calculates the gravitational force between 
 * two objects and updates the first object's 
 * acceleration accordingly.
 */ 
void apply_grav_force(object_t* o1, object_t* o2) {
	if (o1->m == 0 || o2->m == 0) return;

	double dist = distance(o1,o2);
	double sqrd_dist = pow(dist, 2);
	double a = GRAV_CONST * o2->m / sqrd_dist;	/* a = GM/r² */
	
	double dx = abs(o2->x - o1->x);
	double dy = abs(o2->y - o1->y);
	double theta;

	double ax;
	double ay;

	if (o1->y < o2->y) {		/* o1 above */
		theta = atan(dx/dy);
		ax = a * sin(theta);
		ay = a * cos(theta);

	} else if (o1->y > o2->y) {	/* o1 under */
		theta = atan(dy/dx);
		ax = a * cos(theta);
		ay = -a * sin(theta);
	} else if (o1->y == o2->y) {	/* Equal heights */
		ay = 0;
		ax = a;
	}
	if (o1->x > o2->x) {		/* o1 to the right*/
		ax = -ax;
	}

	o1->ax += ax;
	o1->ay += ay;
}

/*
 * Performs the physics of one time quantum
 */
void tick(void) {
	int i,j;
	for (i=0; i<n_objects; i++) {
		/* Determines gravity acceleration*/
		objects[i]->ax = 0;
		objects[i]->ay = 0;
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
			apply_grav_force(objects[i], objects[j]);
		}
		/* Applies acceleration to speed */
		objects[i]->vx += objects[i]->ax;
		objects[i]->vy += objects[i]->ay;

		/* Moves */
		objects[i]->x += objects[i]->vx;
		objects[i]->y += objects[i]->vy;

	}
}
