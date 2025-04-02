#include <stdio.h>
#include "claves/claves.h"

int main() {
    int key = 2;
    char v1[] = "distribuidos";
    double v2[] = {3.1, 6.2, 9.3};
    int N_v2 = 3;
    struct Coord v3 = {20, 10};

    char v1_get[256];
    double v2_get[32];
    int N_v2_get;
    struct Coord v3_get;

    char v1_mod[] = "valor_modificado";
    double v2_mod[] = {1.1, 3.3, 5.5, 7.7};
    int N_v2_mod = 4;
    struct Coord v3_mod = {20, 10};

    printf("\n** Prueba Cliente 2: set_value **\n");
    if (set_value(key, v1, N_v2, v2, v3) == 0) {
        printf("Clave %d insertada correctamente.\n", key);
    } else {
        printf("Error al insertar la clave %d.\n", key);
    }

    printf("\n** Prueba: get_value(key) **\n");
    if (get_value(key, v1_get, &N_v2_get, v2_get, &v3_get) == 0) {
        printf("Clave %d recuperada correctamente.\n", key);
        printf("   - value1: %s\n", v1_get);
        printf("   - N_value2: %d\n", N_v2_get);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get; i++) {
            printf("%lf ", v2_get[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get.x, v3_get.y);
    } else {
        printf("Error al recuperar la clave %d.\n", key);
    }
    
    if (modify_value(key, v1_mod, N_v2_mod, v2_mod, v3_mod) == 0) {
        printf("Clave %d modificada correctamente.\n", key);
    } else {
        printf("Error al modificar la clave %d.\n", key);
    }

    printf("\n** Prueba Cliente 2: delete_key **\n");
    if (delete_key(key) == 0) {
        printf("Clave %d eliminada correctamente.\n", key);
    } else {
        printf("Error al eliminar la clave %d.\n", key);
    }
    return 0;
}
