#include "gravity.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h> /* usleep() */
#include <string.h>


object *objects;
int n_objects;

int array_height = 22;
int array_width = 40;
char centering = 0;

double distance(object *o1, object *o2){
	double dx = o2->x - o1->x;
	double dy = o2->y - o1->y;
	
	double d_squared = pow(dx, 2) + pow(dy, 2);
	return sqrt(d_squared);
}

/*
 * Determines if o1 and o2 are intersecting
 */
char intersects(object *o1, object *o2) {
	double dist = distance(o1,o2);
	if (dist < (o1->r + o2->r)) {
		return 1;
	}
	return 0;
}

/*
 * Calculates the gravitational force between 
 * two objects and updates the first objects' 
 * acceleration accordingly.
 */ 
void apply_grav_force(object *o1, object *o2) {
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

void tick() {
	int i,j;
	for (i=0; i<n_objects; i++) {
		/* Gravity acceleration*/
		objects[i].ax = 0;
		objects[i].ay = 0;
		for (j=0; j<n_objects; j++) {
			if (i==j) continue;
			apply_grav_force(&objects[i], &objects[j]);
		}
		/* Accelerates */
		objects[i].vx += objects[i].ax;
		objects[i].vy += objects[i].ay;

		/* Moves */
		objects[i].x += objects[i].vx;
		objects[i].y += objects[i].vy;
	}

}


void print() {
	int x[n_objects];
	int y[n_objects];
	
	/* Sets first object */
	if (n_objects > 0) {
		x[0] = nearbyint(objects[0].x);
		y[0] = nearbyint(objects[0].y);
	}
	
	/* Determines offset */
	int offset_x = 0;
	int offset_y = 0;
	if (centering) {
		offset_x = (array_width/2) - x[0];
		offset_y = (array_height/2) - y[0];
	}

	/* Applies offset for first object */
	x[0] += offset_x;
	y[0] += offset_y;

	/* Sets all objects and applies offset */
	int i;
	for (i=1; i<n_objects; i++) {
		x[i] = nearbyint(objects[i].x) + offset_x;
		y[i] = nearbyint(objects[i].y) + offset_y;
	}

	/* Prints the universe */
	int j,n;
	char c;
	for (i=0; i<array_height; i++) {
		for (j=0; j<array_width; j++) {
			c = ' '; 
			
			/* Prints border */
			if (j == 0 || j == array_width -1) {
				c = '|';
			} else if (i == 0 || i == array_height -1) {
				c = '-';
			}
			
			/* Prints objects */
			for (n=0; n<n_objects; n++) {
				if (j == x[n] && i == y[n]) {
					c = '0'+n;
				}
			}
			printf("%c ", c);
		}
		printf("\n");
	}
	printf("\n");
}

void create_object(int i, double x, double y, double m, double r,
		   double vx, double vy, double ax, double ay) {

	objects[i].x = x;
	objects[i].y = y;
	objects[i].m = m;
	objects[i].r = r;
	objects[i].vx = vx;
	objects[i].vy = vy;
	objects[i].ax = ax;
	objects[i].ay = ay;
}

void add_default_objects() {
	n_objects = 3;
	objects = (object*) malloc(sizeof(object)*n_objects);
	/*objects[0].x = 4;
	objects[0].y = 10;
	objects[0].m = 1.5;
	objects[0].r = 1;
	objects[0].vx = -0.5;
	objects[0].vy = 1;
	objects[0].ax = 0;
	objects[0].ay = 0;
	*/
	create_object(1,4,10,1.5,1,-0.5,1,0,0);

	objects[0].x = 16;
	objects[0].y = 10;
	objects[0].m = 1000.0;
	objects[0].r = 2.5;
	objects[0].vx = 0;
	objects[0].vy = 0;
	objects[0].ax = 0;
	objects[0].ay = 0;

	objects[2].x = 16;
	objects[2].y = 50;
	objects[2].m = 0.7;
	objects[2].r = 0.9;
	objects[2].vx = -0.3;
	objects[2].vy = -0.3;
	objects[2].ax = 0;
	objects[2].ay = 0;
}

int add_custom_objects() {
	n_objects = 0;

	int i = 0;
	printf("Adding custom objects...\n");
	while (1) {
		size_t new_size = sizeof(object)*(++n_objects);
		printf("new_size: %d\n", new_size);
		object *tmp = (object*)realloc(objects, new_size);
		if (tmp) {
			objects = tmp;
		} else {
			printf("Reallocating of memory for the %dth object failed.\n", i);
			printf("Exiting...\n");
			return -1;
		}
		printf("Enter x posision: ");
		objects[i].x = double_from_user();
		printf("Enter y posision: ");
		objects[i].y = double_from_user();
		printf("Enter mass: ");
		objects[i].m = double_from_user();
		printf("Enter radius: ");
		objects[i].r = double_from_user();
		printf("Enter x velocity: ");
		objects[i].vx = double_from_user();
		printf("Enter y velocity: ");
		objects[i].vy = double_from_user();
		objects[i].ax = 0;
		objects[i].ay = 0;
		printf("Type Y to add more, enter to start simulation... ");
		char c = getchar();
		if (c != 'Y' && c != 'y') {
			printf("\n");
			break;
		}
		getchar();
	}
	printf("n_objects: %d\n", n_objects);
	return 0;
}

int main(int argc, char *argv[]) {
	int i=1;
	int sleep = 20000;
	int delay = 0;
	
	char add_objects = 0;
	while (argc > i) {
		if (!strcmp(argv[i], "--help")) {
			printf("Commands:\n");
			printf("-d x\t Delays x ticks into simulation before start.\n");
			printf("-s x\t Sleeps x microseconds between each tick. \n");
			printf("-h x\t Sets height to x. \n");
			printf("-w x\t Sets witdh to x. \n");
			printf("-c\t Always displays the first object in the center.\n");
			printf("-o\t Add your own objects. \n");
			return -1;
		}
		if (!strcmp(argv[i], "-d")) {
			delay = atoi(argv[++i]);
			printf("delay set to %d\n", delay);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-s")) {
			sleep = atoi(argv[++i]);
			printf("sleep set to %d\n", sleep);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-h")) {
			array_height = atoi(argv[++i]);
			printf("height set to %d\n", array_height);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-w")) {
			array_width = atoi(argv[++i]);
			printf("width set to %d\n", array_width);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-c")) {
			centering = 1;
			printf("centering enabled\n");
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-o")) {
			add_objects = 1;
			printf("adding custom objects\n");
			i++;
			continue;
		}
	}
	
	if (add_objects) {
		int rc = add_custom_objects();
		if (rc)	return rc;	
	} else {
		add_default_objects();
	}
	
	printf("Added following objects:\n");
	print_object_values(&objects[0]);
	print_object_values(&objects[1]);
	print_object_values(&objects[2]);
	usleep(1000000);
	for (i=0; i<delay; i++) tick();
	while (1) {
		print();
		tick();
		usleep(sleep);
	}
	
	return 0;
}

void print_object_values(object *object) {
	printf("x position: \t%f\n", object->x);
	printf("y position: \t%f\n", object->y);
	printf("mass: \t\t%f\n", object->m);
	printf("radius: \t%f\n", object->r);
	printf("x velocity: \t%f\n", object->vx);
	printf("y velocity: \t%f\n", object->vy);
	printf("x acceleration: %f\n", object->ax);
	printf("y acceleration: %f\n", object->ay);
}

double double_from_user() {
	int i=0;
	char c;
	char word[sizeof(char) * 64];
	for (i=0; i<sizeof(char)*64; i++) {
		word[i] = 0;
	}
	i=0;
	while ((c=getchar()) != '\n') {
		word[i++] = c;
	}
	double n = atof(word);
	return n;
}

