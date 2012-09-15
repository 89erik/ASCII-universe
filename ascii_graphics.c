#include "gravity.h"
#include <stdio.h>
#include <math.h>

int array_height = 22;
int array_width = 40;

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

