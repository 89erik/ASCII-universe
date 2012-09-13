#ifndef GRAVITY_H /* avoiding header recursion */
#define GRAVITY_H

#define TRUE 1
#define FALSE 0

extern int n_objects;
extern struct object *objects;
extern char centering;
extern int center_object;


struct object {
	double 	x;	/* Position X */
	double	y;	/* Position Y */
	double 	m;	/* Mass */
	double 	r; 	/* Radius */
	double 	vx;	/* Velocity X */
	double 	vy;	/* Velocity Y */
	double 	ax;	/* Acceleration X */
	double 	ay;	/* Acceleration Y */
};

void print();
void print_object_values(struct object *o);
void loop(int sleep);
void tick();
struct object *create_object(int i, double x, double y, double m, double r,
		   double vx, double vy, double ax, double ay);



#endif
