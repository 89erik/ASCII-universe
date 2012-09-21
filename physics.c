#include "gravity.h"
#include "physics.h"
#include <math.h>
#include <stdlib.h>


extern int n_objects;
extern struct object *objects;

/*
 * Calculates the distance between two objects
 */
double distance(struct object *o1, struct object *o2){
	double dx = o2->x - o1->x;
	double dy = o2->y - o1->y;

	return sqrt(pow(dx, 2) + pow(dy, 2));
}

/*
 * Determines if o1 and o2 are intersecting
 */
char intersects(struct object *o1, struct object *o2) {
	double dist = distance(o1,o2);
	if (dist < (o1->r + o2->r)) {
		return TRUE;
	}
	return FALSE;
}

/*
 * Calculates the gravitational force between 
 * two objects and updates the first object's 
 * acceleration accordingly.
 */ 
void apply_grav_force(struct object *o1, struct object *o2) {
	if (o1->m == 0 || o2->m == 0) return;
	if (intersects(o1, o2)) return;

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
		objects[i].ax = 0;
		objects[i].ay = 0;
		for (j=0; j<n_objects; j++) {
			if (i==j) continue;
			apply_grav_force(&objects[i], &objects[j]);
		}
		/* Applies acceleration to speed */
		objects[i].vx += objects[i].ax;
		objects[i].vy += objects[i].ay;

		/* Moves */
		objects[i].x += objects[i].vx;
		objects[i].y += objects[i].vy;
	}
}
