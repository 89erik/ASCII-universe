#include "vector.h"

#include <math.h>

double f_vec_euclidean_distance(f_vec_t v1, f_vec_t v2) {
    f_vec_t d;
    d.x = v2.x - v1.x;
    d.y = v2.y - v1.y;

    return sqrt(pow(d.x, 2) + pow(d.y, 2));
}

f_vec_t f_vec_add(f_vec_t v1, f_vec_t v2) {
    f_vec_t sum;
    sum.x = v1.x + v2.x;
    sum.y = v1.y + v2.y;
    return sum;
}

f_vec_t f_vec(double x, double y) {
    return (f_vec_t) {x,y};
}

void f_vec_accumulate(f_vec_t* v1, f_vec_t* v2) {
    v1->x += v2->x;
    v1->y += v2->y;
}

void i_vec_accumulate(i_vec_t* v1, i_vec_t* v2) {
    v1->x += v2->x;
    v1->y += v2->y;
}

f_vec_t f_vec_diff(f_vec_t v1, f_vec_t v2) {
    f_vec_t diff;
    diff.x = v2.x - v1.x;
    diff.y = v2.y - v1.y;
    f_vec_abs(&diff);
    return diff;
}

void f_vec_abs(f_vec_t* v) {
    v->x = fabs(v->x);
    v->y = fabs(v->y);
}

i_vec_t vec_ftoi(f_vec_t fv) {
    return (i_vec_t) {nearbyint(fv.x), nearbyint(fv.y)};
}

#ifdef TESTING
void main() {
    f_vec_t f = f_vec(1.9, 1.1);
    i_vec_t i = vec_ftoi(f);
    printf("%d, %d\n", i.x,i.y);
}
#endif

