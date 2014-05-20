#include <stdlib.h>
#include <stdbool.h>

#include "guitraildot.h"

#define TRAIL_DOTS_INITIAL_SIZE 512
#define TRAIL_DOTS_MAX_SIZE     512*40

       bool     trail = false;
       i_vec_t* trail_dots;
       int      n_trail_dots = 0;
static int      trail_dots_size = 0;


void insert_trail_dot(i_vec_t trail_dot) {
    static int i = 0;
    if (!(n_trail_dots+1 < trail_dots_size)) {
        if (trail_dots_size == 0) {
            i = 0;
            trail_dots_size = TRAIL_DOTS_INITIAL_SIZE;
        } else { 
            trail_dots_size *= 2;
            if (trail_dots_size > TRAIL_DOTS_MAX_SIZE) {
                trail_dots_size = TRAIL_DOTS_MAX_SIZE;
            }
        }
        trail_dots = (i_vec_t*) realloc(trail_dots, sizeof(i_vec_t) * trail_dots_size);
    }

    if (n_trail_dots+1 < TRAIL_DOTS_MAX_SIZE) {
        n_trail_dots++;
    }
    trail_dots[i] = trail_dot;
    i = i+1 < TRAIL_DOTS_MAX_SIZE? i+1 : 0;
}

void clear_trail_dots() {
    n_trail_dots = 0;
    trail_dots_size = 0;
    free(trail_dots);
    trail_dots = NULL;
}

