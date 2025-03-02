#ifndef _MENSAJE_H_
#define _MENSAJE_H_

#define MAX 256

struct message {
    int op;
    char q_name[MAX];
    int key;
    char value1[MAX];
    int N_value2;
    double V_value2;
    /* No se incluye nada relativo a value3 porque éste
    va incluido ya en el servidor, una vez se importa la librería claves.h*/
    /*char *ptr1;
    int *ptr2;
    double *ptr3;*/
    int res;
};

#endif