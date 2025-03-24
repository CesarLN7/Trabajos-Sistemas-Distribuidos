#include <stdio.h>
#include "claves/claves.h"

int main() {
    int key = 2;
    char v1[] = "distribuidos";
    double v2[] = {3.1, 6.2, 9.3};
    int N_v2 = 3;
    struct Coord v3 = {20, 10};

    printf("\n** Prueba Cliente 2: set_value **\n");
    if (set_value(key, v1, N_v2, v2, v3) == 0) {
        printf("Clave %d insertada correctamente.\n", key);
    } else {
        printf("Error al insertar la clave %d.\n", key);
    }

    printf("\n** Prueba Cliente 2: delete_key **\n");
    if (delete_key(key) == 0) {
        printf("Clave %d eliminada correctamente.\n", key);
    } else {
        printf("Error al eliminar la clave %d.\n", key);
    }
    return 0;
}
