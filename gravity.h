#ifndef GRAVITY_H
#define GRAVITY_H

#define TRUE 1
#define FALSE 0

#include "physics.h"


void print_object_values(struct object *o);
void loop(int sleep, long print_delay);

//struct object *create_object(int i, double x, double y, double m, double r, double vx, double vy, double ax, double ay);



#endif
