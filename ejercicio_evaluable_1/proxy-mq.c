/*implementar funciones de claves.h (al principio vacías). -2 cuando fallo*/
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include "claves/claves.h"


int destroy() { return 1; };

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { return 1; };

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) { return 1; };

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { return 1; };

int delete_key(int key) { return 1; };

int exist(int key) { return 1; };