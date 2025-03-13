#include <stdio.h>
#include "claves/claves.h"

int main() {
    int key = 5;
    char v1_set[] = "valor_inicial";
    double v2_set[] = {2.3, 0.5, 23.45};
    int N_v2_set = 3;
    struct Coord v3_set = {10, 5};

    char v1_get[256];
    double v2_get[32];
    int N_v2_get;
    struct Coord v3_get;

    // 1. Destruir cualquier dato previo
    printf("\n** Prueba: destroy() **\n");
    if (destroy() == 0) {
        printf("Servicio inicializado correctamente.\n");
    } else {
        printf("Error al destruir las tuplas anteriores.\n");
    }

    // 2. Comprobar que la clave no existe aún
    printf("\n** Prueba: exist(key) antes de set_value **\n");
    int exists = exist(key);
    if (exists == 0) {
        printf("La clave %d no existe (correcto).\n", key);
    } else {
        printf("Error: la clave %d existe antes de ser insertada.\n", key);
    }

    // 3. Insertar la clave con set_value
    printf("\n** Prueba: set_value(key, v1, N_v2, v2, v3) **\n");
    if (set_value(key, v1_set, N_v2_set, v2_set, v3_set) == 0) {
        printf("Clave %d insertada correctamente.\n", key);
    } else {
        printf("Error al insertar la clave %d.\n", key);
    }

    printf("\n** Prueba: set_value(key, v1, N_v2, v2, v3) **\n");
    if (set_value(2, v1_set, N_v2_set, v2_set, v3_set) == 0) {
        printf("Clave %d insertada correctamente.\n", key);
    } else {
        printf("Error al insertar la clave %d.\n", key);
    }

    // 4. Comprobar que ahora la clave sí existe
    printf("\n** Prueba: exist(key) después de set_value **\n");
    exists = exist(key);
    if (exists == 1) {
        printf("La clave %d existe (correcto).\n", key);
    } else {
        printf("Error: la clave %d no existe después de ser insertada.\n", key);
    }

    // 5. Obtener los valores de la clave
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

    // 6. Modificar los valores de la clave
    printf("\n** Prueba: modify_value(key, nuevos valores) **\n");
    char v1_mod[] = "valor_modificado";
    double v2_mod[] = {1.1, 3.3, 5.5, 7.7};
    int N_v2_mod = 4;
    struct Coord v3_mod = {20, 10};

    if (modify_value(key, v1_mod, N_v2_mod, v2_mod, v3_mod) == 0) {
        printf("Clave %d modificada correctamente.\n", key);
    } else {
        printf("Error al modificar la clave %d.\n", key);
    }

    // 7. Obtener los valores después de modificar
    printf("\n** Prueba: get_value(key) después de modificar **\n");
    if (get_value(key, v1_get, &N_v2_get, v2_get, &v3_get) == 0) {
        printf("Clave %d recuperada después de modificar.\n", key);
        printf("   - value1: %s\n", v1_get);
        printf("   - N_value2: %d\n", N_v2_get);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get; i++) {
            printf("%lf ", v2_get[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get.x, v3_get.y);
    } else {
        printf("Error al recuperar la clave %d después de modificar.\n", key);
    }

    // 8. Eliminar la clave
    printf("\n** Prueba: delete_key(key) **\n");
    if (delete_key(key) == 0) {
        printf("Clave %d eliminada correctamente.\n", key);
    } else {
        printf("Error al eliminar la clave %d.\n", key);
    }

    // 9. Comprobar que la clave ya no existe
    printf("\n** Prueba: exist(key) después de delete_key **\n");
    exists = exist(key);
    if (exists == 0) {
        printf("La clave %d ya no existe (correcto).\n", key);
    } else {
        printf("Error: la clave %d aún existe después de ser eliminada.\n", key);
    }

    return 0;
}
