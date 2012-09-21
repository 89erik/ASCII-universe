#ifndef PHYSICS_H
#define PHYSICS_H

#define GRAV_CONST 0.01
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif 


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


double distance(struct object *o1, struct object *o2);
char intersects(struct object *o1, struct object *o2);
void apply_grav_force(struct object *o1, struct object *o2);
void tick(void);


#endif
