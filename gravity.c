#include "gravity.h"
#include "user_input.h"	
#include "ascii_graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>


char centering = FALSE;
int center_object = 0;

int n_objects;
struct object *objects;

int offset_x = 0;
int offset_y = 0;


/* 
 * Collects arguments from the user and starts simulation
 */
int main(int argc, char *argv[]) {
	int rc, i=1;
	int delay = 0;
	int sleep = 20000;
	long print_delay = 20000000;
	char add_objects = FALSE;

	extern int array_height;
	extern int array_width;
	
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
	loop(sleep, print_delay);
	
	return 0;
}

/*
 * Runs the simulation.
 * Performs an eternal loop with 3 main stages:
 *     1. Perform the physics of one time quantum
 *     2. Sleep for the given amount of time
 *     3. Draw the screen (if enough time has passed 
 *        according to the given print delay)
 */
void loop(int sleep, long print_delay) {
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

/*
 * Prints the values of a given object
 */
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


/*
 * Creates and initializes an object
 *//*
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
}*/

