#include "gravity.h"
//#include "user_input.h"	
//#include "physics.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

int array_height = 22;
int array_width = 40;

char centering = 0;
int center_object = 0;

int n_objects;
struct object *objects;

int offset_x = 0;
int offset_y = 0;

long print_delay = 20000000;


void print() {
	int x[n_objects];	/* rounded x-pos */
	int y[n_objects];	/* rounded y-pos */
	int r[n_objects];	/* rounded radius */
	
	/* Sets centered object */
	if (n_objects > 0) {
		x[center_object] = nearbyint(objects[center_object].x);
		y[center_object] = nearbyint(objects[center_object].y);
		r[center_object] = nearbyint(objects[center_object].r);
	}
	
	/* Determines offset for centering */
	if (centering) {
		offset_x = (array_width/2) - x[center_object];
		offset_y = (array_height/2) - y[center_object];
	}

	/* Sets objects and applies offset */
	int i;
	for (i=0; i<n_objects; i++) {
		x[i] = nearbyint(objects[i].x) + offset_x;
		y[i] = nearbyint(objects[i].y) + offset_y;
		r[i] = nearbyint(objects[i].r);
	}

	/* Prints the universe... */
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
				
				/* Center of object */
				if (j == x[n] && i == y[n]) {
					c = '0'+n;
					continue;
				}
				
				/* Non-center of object */
				char right = (j >= (x[n] - r[n]));
				char left  = (j <= (x[n] + r[n]));
				char over  = (i >= (y[n] - r[n]));
				char under = (i <= (y[n] + r[n]));

				if (right && left && over && under) {
					c = '0'+n;
				}
			}
			printf("%c ", c);
		}
		printf("\n");
	}
	printf("\n");
}



void add_default_objects() {
	n_objects = 3;
	objects = (struct object*) malloc(sizeof(struct object)*n_objects);

	objects[0].x = 16;
	objects[0].y = 10;
	objects[0].m = 1000.0;
	objects[0].r = 2.5;
	objects[0].vx = 0;
	objects[0].vy = 0;
	objects[0].ax = 0;
	objects[0].ay = 0;

	objects[1].x = 4;
	objects[1].y = 10;
	objects[1].m = 1.5;
	objects[1].r = 1;
	objects[1].vx = -0.5;
	objects[1].vy = 1;
	objects[1].ax = 0;
	objects[1].ay = 0;

	objects[2].x = 16;
	objects[2].y = 50;
	objects[2].m = 0.7;
	objects[2].r = 0.9;
	objects[2].vx = -0.3;
	objects[2].vy = -0.3;
	objects[2].ax = 0;
	objects[2].ay = 0;
}



int main(int argc, char *argv[]) {
	int rc, i=1;
	int sleep = 20000;
	int delay = 0;

	clock_t c0, c1;
	time_t  t0, t1;
	c0 = clock();
	t0 = time(NULL);

	char add_objects = FALSE;
	while (argc > i) {
		if (!strcmp(argv[i], "--help")) {
			printf("Commands:\n");
			printf("Speed:\n");
			printf("-d x\t Delays x ticks into simulation before start.\n");
			printf("-s x\t Sleeps x microseconds between each tick. \n");
			printf("\t Shorter sleep = faster simulation. Default is 20000 µs\n");
			printf("-pd x\t Waits x nanoseconds between each print. \n");
			printf("\t Shorter wait = faster framerate. Default is 20000000 ns\n");
		
			printf("\nDisplay:\n");
			printf("-h x\t Sets print area height to x. \n");
			printf("-w x\t Sets print area width to x. \n");
			printf("-c x\t Centering. Always display object #x (zero indexed) in the center.\n");
			printf("\t The centered object will appear stationary in the center.\n");
			printf("-do x y\t Draws with offset x,y (overruled by centering)\n");
			printf("\t x,y will be the top left point\n");
			printf("\nSimulation:\n");			
			printf("-o\t Add your own objects. \n");
			return 0;
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
			if (i < argc-1) {
				if (argv[i+1][0] != '-') {
					center_object = atoi(argv[++i]);
				}
			}
			printf("centering enabled on object #%d\n", center_object);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-do")) {
			if (i < argc-2) {
				if (argv[i+1][0] != '-' && argv[i+2][0] != '-') {
					offset_x = atoi(argv[++i]);
					offset_y = atoi(argv[++i]);
				}
			}
			printf("Draws screen with offset (%d,%d)\n", offset_x, offset_y);
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-o")) {
			add_objects = 1;
			printf("adding custom objects\n");
			i++;
			continue;
		}
		if (!strcmp(argv[i], "-pd")) {
			if (i < argc-1) {
				if (argv[i+1][0] != '-') {
					print_delay = atol(argv[++i]);
				}
			}
			printf("Print delay set to %ld\n", print_delay);
			i++;
			continue;
		}
		i++;
	}

	if (add_objects) {
		int rc = add_custom_objects();
		if (rc)	return rc;
	} else {
		add_default_objects();
	}
	
	printf("\nSimulating following objects:\n");
	for (i=0; i<n_objects; i++) {
		print_object_values(&objects[i]);
		printf("\n");
	}

	usleep(1000000);
	for (i=0; i<delay; i++) tick();

	loop(sleep);
	
	return 0;
}

void loop(int sleep) {
	int rc;
	clockid_t clk_id = CLOCK_REALTIME;
	struct timespec *res = malloc(sizeof(struct timespec));
	clock_getres(clk_id, res);
	
	rc = clock_gettime(clk_id, res);
	long last_s = res->tv_sec;
	long last_ns = res->tv_nsec;
	long current_s;
	long current_ns;
	unsigned long long delta;

	while (TRUE) {
		tick();
		usleep(sleep);


		rc = clock_gettime(clk_id, res);
		current_s = res->tv_sec;
		current_ns = res->tv_nsec;
		
		delta = current_s - last_s;
		delta *= 1000000000;
		delta += current_ns - last_ns;

		if (delta > print_delay) {
			print();

			rc = clock_gettime(clk_id, res);
			last_s = res->tv_sec;
			last_ns = res->tv_nsec;
		}
	}
	free(res);
}

void print_object_values(struct object *o) {
	printf("x position: \t%f\n", o->x);
	printf("y position: \t%f\n", o->y);
	printf("mass: \t\t%f\n", o->m);
	printf("radius: \t%f\n", o->r);
	printf("x velocity: \t%f\n", o->vx);
	printf("y velocity: \t%f\n", o->vy);
	printf("x acceleration: %f\n", o->ax);
	printf("y acceleration: %f\n", o->ay);
}



/* Object*/ 


void tick() {
	int i,j;
	for (i=0; i<n_objects; i++) {
		/* Gravity acceleration*/
		objects[i].ax = 0;
		//set_ax(&objects[i], 0);
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


struct object *create_object(int i, double x, double y, double m, double r,
		   double vx, double vy, double ax, double ay) {

	struct object *o = malloc(sizeof(struct object));
	o->x = x;
	o->y = y;
	o->m = m;
	o->r = r;
	o->vx = vx;
	o->vy = vy;
	o->ax = ax;
	o->ay = ay;
	return o;
}

