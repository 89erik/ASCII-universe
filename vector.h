#ifndef VECTOR_H
#define VECTOR_H

typedef struct i_vec {
    int x;
    int y;
} i_vec_t;

typedef struct f_vec {
    double x;
    double y;
} f_vec_t;

double f_vec_euclidean_distance(f_vec_t v1, f_vec_t v2);
f_vec_t f_vec_add(f_vec_t v1, f_vec_t v2);
void f_vec_accumulate(f_vec_t* v1, f_vec_t* v2);
f_vec_t f_vec_diff(f_vec_t v1, f_vec_t v2);
void f_vec_abs(f_vec_t* v);
f_vec_t f_vec(double x, double y);
i_vec_t vec_ftoi(f_vec_t fv);

#endif
