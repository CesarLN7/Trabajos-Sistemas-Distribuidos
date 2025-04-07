#ifndef _PROXY_SOCK_H_
#define _PROXY_SOCK_H_

#include "claves/claves.h"  // Para tener la definición de struct Coord

int destroy();

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);

int delete_key(int key);

int exist(int key);

#endif
