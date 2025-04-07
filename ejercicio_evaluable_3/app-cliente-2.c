#include <stdio.h>
#include "claves/claves.h"

int main() {
    int key_client2 = 100;
    
    char v1_client2[] = "cliente2";
    double v2_client2[] = {1.1, 2.2, 3.3};
    int N_v2_client2 = 3;
    struct Coord v3_client2 = {5, 10};

    char v1_get[256];
    double v2_get[32];
    int N_v2_get;
    struct Coord v3_get;

    // 1. Comprobar que la clave no existe
    printf("\n** Prueba: exist() antes de set_value **\n");
    int exists = exist(key_client2);
    if (exists == 0) {
        printf("La clave %d no existe (correcto).\n", key_client2);
    } else if (exists == 1) {
        printf("Error: la clave %d existe antes de ser insertada.\n", key_client2);
    } else {
        printf("Error al comprobar la clave %d.\n", key_client2);
    }
    
    // 2. Insertar valores con set_value
    printf("\n** Prueba: set_value() **\n");
    if (set_value(key_client2, v1_client2, N_v2_client2, v2_client2, v3_client2) == 0) {
        printf("Clave %d insertada correctamente.\n", key_client2);
    } else {
        printf("Error al insertar la clave %d.\n", key_client2);
    }
 
    
    // 3. Obtener valores con get_value
    printf("\n** Prueba: get_value() **\n");
    if (get_value(key_client2, v1_get, &N_v2_get, v2_get, &v3_get) == 0) {
        printf("Clave %d recuperada después de modificar.\n", key_client2);
        printf("   - value1: %s\n", v1_get);
        printf("   - N_value2: %d\n", N_v2_get);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get; i++) {
            printf("%lf ", v2_get[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get.x, v3_get.y);
    } else {
        printf("Error al recuperar la clave %d.\n", key_client2);
    }
    
    // 4. Modificar valores con modify_value
    printf("\n** Prueba: modify_value() **\n");
    char v1_mod[] = "modificado_cliente2";
    double v2_mod[] = {10.10, 11.11, 12.12};
    int N_v2_mod = 3;
    struct Coord v3_mod = {35, 40};
    if (modify_value(key_client2, v1_mod, N_v2_mod, v2_mod, v3_mod) == 0) {
        printf("Clave %d modificada correctamente.\n", key_client2);
    } else {
        printf("Error al modificar la clave %d.\n", key_client2);
    }
    
    // 5. Obtener valores modificados con get_value
    printf("\n** Prueba: get_value() después de modify_value **\n");
    if (get_value(key_client2, v1_get, &N_v2_get, v2_get, &v3_get) == 0) {
        printf("Clave %d recuperada después de modificar.\n", key_client2);
        printf("   - value1: %s\n", v1_get);
        printf("   - N_value2: %d\n", N_v2_get);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get; i++) {
            printf("%lf ", v2_get[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get.x, v3_get.y);
    } else {
        printf("Error al recuperar la clave %d después de modificar.\n", key_client2);
    }
    
    return 0;
}
