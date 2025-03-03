#ifndef _PROXY_MQ_H_
#define _PROXY_MQ_H_

int q_destroy();

int q_set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);

int q_get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);

int q_modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);

int q_delete_key(int key);

int q_exist(int key);

#endif