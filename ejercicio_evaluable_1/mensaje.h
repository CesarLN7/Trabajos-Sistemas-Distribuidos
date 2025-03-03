#ifndef _MENSAJE_H_
#define _MENSAJE_H_

#define MAXSTR 256
#define MAXVEC 32

struct message {
    int op;
    char q_name[MAXSTR];
    int key;
    char value1[MAXSTR];
    int N_value2[MAXVEC];
    double V_value2;
    struct Coord *value3;
    /*char *ptr1;
    int *ptr2;
    double *ptr3;*/
    int res;
};

#endif