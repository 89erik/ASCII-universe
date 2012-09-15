#ifndef PHYSICS_H /* avoiding header recursion */
#define PHYSICS_H

#define GRAV_CONST 0.01
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif 


double distance(struct object *o1, struct object *o2);
char intersects(struct object *o1, struct object *o2);
void apply_grav_force(struct object *o1, struct object *o2);
void tick();


#endif
