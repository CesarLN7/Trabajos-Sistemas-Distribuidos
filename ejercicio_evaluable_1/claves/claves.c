/* lógica almacenamiento de las funciones. return -1 cuando fallo. Impementar lista enlazada*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int destroy() {
    FILE *f = fopen("data.txt", "r");
    // create the file if it does not exist
    if (f == NULL) {
        f = fopen("data.txt", "w");
        fclose(f);
    } else {
        fclose(f);
    }
    return 0;
}

/*int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { return 1; };

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) { return 1; };

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { return 1; };

int delete_key(int key) { return 1; };

int exist(int key) { return 1; };*/