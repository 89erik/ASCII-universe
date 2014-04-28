#include "physics.h"
#include "gravity.h"
#include "object.h"

#include <stdio.h>
#include <stdlib.h>

#define STRING_MAX 64

extern int n_objects;
extern object_t** objects;
extern char centering;
extern int center_object;


int double_from_user(double *from_user) {
	int i=0;
	char c;
	char word[STRING_MAX];
	for (i=0; i<STRING_MAX; i++) {
		word[i] = 0;
	}
	i=0;
	while ((c=getchar()) != '\n') {
		word[i++] = c;
		if ( ((c-'0') > 9 || (c-'0') < 0) && c != '.' && c != '-') {
			printf("Input must be a number using '.' as decimal\n");
			while ((c=getchar()) != '\n'); /* Skips to end of line */
			return -1;
		}
	}
	*from_user = atof(word);
	return 0;
}

int int_from_user(int *from_user) {
	int i=0;
	char c;
	char word[STRING_MAX];
	for (i=0; i<STRING_MAX; i++) {
		word[i] = 0;
	}
	i=0;
	while ((c=getchar()) != '\n') {
		word[i++] = c;
		if ( ((c-'0') > 9 || (c-'0') < 0) && c != '-') {
			printf("Input must be an integer number\n");
			while ((c=getchar()) != '\n'); /* Skips to end of line */
			return -1;
		}
	}
	*from_user = atoi(word);
	return 0;
}

/*
 * Allows the user to input objects
 */
int add_custom_objects(void) {
	n_objects = 0;
	object_t** tmp;
	int rc, i = 0;
	double *from_user = malloc(sizeof(double));

	printf("Adding custom objects...\n");
	while (true) {
		
		size_t new_size = sizeof(object_t*) * (++n_objects);
		tmp = (object_t**) realloc(objects, new_size);
		if (tmp) {
			objects = tmp;
		} else {
			printf("Reallocating of memory for the %dth object failed.\n", i);
			printf("Exiting...\n");
			return -1;
		}
		do {
			printf("Enter x posision: ");
			rc = double_from_user(from_user);
			objects[i]->x = *from_user;
		} while (rc);

		do {
			printf("Enter y posision: ");
			rc = double_from_user(from_user);
			objects[i]->y = *from_user;
		} while (rc);

		do {
			printf("Enter mass: ");
			rc = double_from_user(from_user);
			objects[i]->m = *from_user;
		} while (rc);

		do {
			printf("Enter radius: ");
			rc = double_from_user(from_user);
			objects[i]->r = *from_user;
		} while (rc);

		do {
			printf("Enter x velocity: ");
			rc = double_from_user(from_user);
			objects[i]->vx = *from_user;
		} while (rc);

		do {
			printf("Enter y velocity: ");
			rc = double_from_user(from_user);
			objects[i]->vy = *from_user;
		} while (rc);

		objects[i]->ax = 0;
		objects[i]->ay = 0;


		print_object_values(objects[i]);

		printf("Type Y to add more, enter to start simulation... ");
		char c = getchar();
		
		if (c != '\n') {		
			while (getchar() != '\n'); /* Skips to line feed */
		}

		if (c != 'Y' && c != 'y') {		/* No more objects */
			printf("\n");

			if (centering && n_objects <= center_object) {	/* Invalid center object */
				printf("You added %d objects, but centered object was set to %d\n", 
						n_objects, center_object);
				printf("Objects are zero-indexed, meaning the first object is object #0\n");
				int *from_user = malloc(sizeof(int));
				int rc;
				do {
					printf("Enter new center object (less than %d)...", n_objects);			
					rc = int_from_user(from_user);
				} while (rc || n_objects <= *from_user);
				center_object = *from_user;
				free(from_user);
			}
			break;
		}
		i++;
		
	}
	free(from_user);
	return 0;
}

/*
 * Initializes three default objects for the simulation
 */
void add_default_objects(void) {
#define DEFAULT_OBJECTS
#ifdef DEFAULT_OBJECTS
    insert_new_object(0,0,40000,10,0,0);
    insert_new_object(200,200,50,1,-0.8,0.8);
    insert_new_object(100,100,3,0.9,-1,1);
    insert_new_object(-300,-300,3,0.9,1.1,-0.3);

#endif
}
