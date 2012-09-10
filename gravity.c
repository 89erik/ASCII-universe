#include "gravity.h"
//#include "physics.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>



#define TRUE 1

int array_height = 22;
int array_width = 40;
char centering = 0;
int center_object = 0;
int offset_x = 0;
int offset_y = 0;

int n_objects;
object *objects;



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
	objects = (object*) malloc(sizeof(object)*n_objects);

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

int add_custom_objects() {
	n_objects = 0;
	object *tmp;
	int rc, i = 0;
	double *from_user = malloc(sizeof(double));

	printf("Adding custom objects...\n");
	while (TRUE) {
		
		size_t new_size = sizeof(object)*(++n_objects);
//		printf("New size: %d\n", new_size);
		tmp = (object*)realloc(objects, new_size);
		if (tmp) {
			objects = tmp;
		} else {
			printf("Reallocating of memory for the %dth object failed.\n", i);
			printf("Exiting...\n");
			return -1;
		}
		do {
			printf("Enter x posision: ");
			rc = double_from_user(from_user);
			objects[i].x = *from_user;
		} while (rc);

		do {
			printf("Enter y posision: ");
			rc = double_from_user(from_user);
			objects[i].y = *from_user;
		} while (rc);

		do {
			printf("Enter mass: ");
			rc = double_from_user(from_user);
			objects[i].m = *from_user;
		} while (rc);

		do {
			printf("Enter radius: ");
			rc = double_from_user(from_user);
			objects[i].r = *from_user;
		} while (rc);

		do {
			printf("Enter x velocity: ");
			rc = double_from_user(from_user);
			objects[i].vx = *from_user;
		} while (rc);

		do {
			printf("Enter y velocity: ");
			rc = double_from_user(from_user);
			objects[i].vy = *from_user;
		} while (rc);

		objects[i].ax = 0;
		objects[i].ay = 0;


		print_object_values(&objects[i]);

		printf("Type Y to add more, enter to start simulation... ");
		char c = getchar();
		
		if (c != '\n') {		
			while (getchar() != '\n'); /* Skips to line feed */
		}

		if (c != 'Y' && c != 'y') {		/* No more objects */
			printf("\n");

			if (centering && n_objects <= center_object) {	/* Invalid center object */
				printf("You added %d objects, but centered object was set to %d\n", 
						n_objects, center_object);
				printf("Objects are zero-indexed, meaning the first object is object #0\n");
				int *from_user = malloc(sizeof(int));
				int rc;
				do {
					printf("Enter new center object (less than %d)...", n_objects);			
					rc = int_from_user(from_user);
				} while (rc || n_objects <= *from_user);
				center_object = *from_user;
				free(from_user);
			}
			break;
		}
		i++;
		
	}
	free(from_user);
	printf("n_objects: %d\n", n_objects);
	return 0;
}

int main(int argc, char *argv[]) {
	int rc, i=1;
	int sleep = 20000;
	int delay = 0;

	clock_t c0, c1;
	time_t  t0, t1;
	c0 = clock();
	t0 = time(NULL);

	char add_objects = 0;
	while (argc > i) {
		if (!strcmp(argv[i], "--help")) {
			printf("Commands:\n");
			printf("-d x\t Delays x ticks into simulation before start.\n");
			printf("-s x\t Sleeps x microseconds between each tick. \n");
			printf("-h x\t Sets height to x. \n");
			printf("-w x\t Sets witdh to x. \n");
			printf("-c x\t Always displays object #x (zero indexed) in the center.\n");
			printf("-do x y\t Draws the screen with offset x,y (overruled by centering)\n");
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
			if (i < argc-1) {
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

	c0 = clock();
	/*while (1) {
		c1 = clock();
		if ((c1-c0) >= 10000) {
			printf("%d\n", c1-c0);
			c0 = clock();
		}
		
	}*/
	while (TRUE) {
		c1 = clock();
		//if ((c1-c0) >= 1) {
			print();
		//	printf("%d\n",(int)(c1-c0));
		//	c0 = clock();
		//}
		tick();
		usleep(sleep);
	}
	
	return 0;
}

void print_object_values(object *o) {
	printf("x position: \t%f\n", o->x);
	printf("y position: \t%f\n", o->y);
	printf("mass: \t\t%f\n", o->m);
	printf("radius: \t%f\n", o->r);
	printf("x velocity: \t%f\n", o->vx);
	printf("y velocity: \t%f\n", o->vy);
	printf("x acceleration: %f\n", o->ax);
	printf("y acceleration: %f\n", o->ay);
}

int double_from_user(double *from_user) {
	int i=0;
	char c;
	char word[sizeof(char) * 64];
	for (i=0; i<sizeof(char)*64; i++) {
		word[i] = 0;
	}
	i=0;
	while ((c=getchar()) != '\n') {
		word[i++] = c;
		if ( ((c-'0') > 9 || (c-'0') < 0) && c != '.' && c != '-') {
			printf("Input must be a number using '.' as decimal\n");
			while ((c=getchar()) != '\n'); /* Skips to end of line */
			return -1;
		}
	}
	*from_user = atof(word);
	return 0;
}

int int_from_user(int *from_user) {
	int i=0;
	char c;
	char word[sizeof(char) * 64];
	for (i=0; i<sizeof(char)*64; i++) {
		word[i] = 0;
	}
	i=0;
	while ((c=getchar()) != '\n') {
		word[i++] = c;
		if ( ((c-'0') > 9 || (c-'0') < 0) && c != '-') {
			printf("Input must be an integer number\n");
			while ((c=getchar()) != '\n'); /* Skips to end of line */
			return -1;
		}
	}
	*from_user = atoi(word);
	return 0;
}


/* Physics */
/*
 * Calculates the distance between two objects
 */
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


object *create_object(int i, double x, double y, double m, double r,
		   double vx, double vy, double ax, double ay) {

	object *o = /*(*object)*/ malloc(sizeof(object));
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

