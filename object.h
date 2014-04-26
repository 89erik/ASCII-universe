#ifndef OBJECT_H
#define OBJECT_H

typedef struct object {
	double 	x;	/* Position X */
	double	y;	/* Position Y */
	double 	m;	/* Mass */
	double 	r; 	/* Radius */
	double 	vx;	/* Velocity X */
	double 	vy;	/* Velocity Y */
	double 	ax;	/* Acceleration X */
	double 	ay;	/* Acceleration Y */
} object_t;

static void add_object(object_t* object);
void insert_new_object(double x, double y, double m, double r, double vx, double vy);

#endif
