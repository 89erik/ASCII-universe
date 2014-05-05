#ifndef PHYSICS_H
#define PHYSICS_H

#include "object.h"
#include "vector.h"

#include <stdbool.h>

#define GRAV_CONST 0.01
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif 




double distance(object_t* o1, object_t* o2);
bool intersects(object_t* o1, object_t* o2);
void apply_grav_force(object_t* o1, object_t* o2, f_vec_t* acceleration);
void tick(void);


#endif
