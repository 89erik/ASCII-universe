#ifndef GRAVITY_H /* avoiding header recursion */
#define GRAVITY_H


#define GRAV_CONST 0.01
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif 

//#include "object.h"


typedef struct object {
	double 	x;	/* Position X */
	double	y;	/* Position Y */
	double 	m;	/* Mass */
	double 	r; 	/* Radius */
	double 	vx;	/* Velocity X */
	double 	vy;	/* Velocity Y */
	double 	ax;	/* Acceleration X */
	double 	ay;	/* Acceleration Y */
}object;

void print();

int double_from_user(double *from_user);
void print_object_values(object *o);

/* Physics */
double distance(object *o1, object *o2);
char intersects(object *o1, object *o2);
void apply_grav_force(object *o1, object *o2);

/* Object */
void tick();
object *create_object(int i, double x, double y, double m, double r,
		   double vx, double vy, double ax, double ay);



#endif
