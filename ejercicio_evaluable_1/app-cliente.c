#include <stdio.h>
#include "claves/claves.h"

int main() {
    int key = 1;
    char v1_set[] = "sistemas";
    double v2_set[] = {2.3, 0.5, 23.45};
    int N_v2_set = 3;
    struct Coord v3_set = {10, 5};

    int key1 = 2, key2 = 3, key3 = 4, key4 = 5, key5 = 6;
    char v1_set1[] = "distribuidos";
    double v2_set1[] = {2.3, 0.5, 23.45, 13.13, 33.33};
    int N_v2_set1 = 5;
    struct Coord v3_set1 = {23, 33};
    
    char v1_set2[] = "ejercicio";
    double v2_set2[] = {7.1, 9.2};
    int N_v2_set2 = 2;
    struct Coord v3_set2 = {30, 15};
    
    char v1_set3[] = "evaluable1";
    double v2_set3[] = {4.4, 8.8, 12.12, 16.16};
    int N_v2_set3 = 4;
    struct Coord v3_set3 = {25, 50};

    char v1_set4[] = "evaluable1";
    double v2_set4[] = {4.4, 8.8, 12.12, 16.16};
    int N_v2_set4 = 40;
    struct Coord v3_set4 = {25, 50};

    char v1_set5[] = "String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres. String con más de 255 caracteres.";
    double v2_set5[] = {4.4, 8.8, 12.12, 16.16};
    int N_v2_set5 = 4;
    struct Coord v3_set5 = {25, 50};

    char v1_get[256];
    double v2_get[32];
    int N_v2_get;
    struct Coord v3_get;

    char v1_get1[256];
    double v2_get1[32];
    int N_v2_get1;
    struct Coord v3_get1;

    char v1_get2[256];
    double v2_get2[32];
    int N_v2_get2;
    struct Coord v3_get2;

    char v1_get3[256];
    double v2_get3[32];
    int N_v2_get3;
    struct Coord v3_get3;

    char v1_get4[256];
    double v2_get4[32];
    int N_v2_get4;
    struct Coord v3_get4;

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
    printf("\n** Prueba: set_value con diferentes claves **\n");
    if (set_value(key, v1_set, N_v2_set, v2_set, v3_set) == 0) {
        printf("Clave %d insertada correctamente.\n", key);
    } else {
        printf("Error al insertar la clave %d.\n", key);
    }
    if (set_value(key, v1_set, N_v2_set, v2_set, v3_set) == 0) {
        printf("Clave %d insertada correctamente (Error, clave duplicada).\n", key);
    } else {
        printf("Error al insertar la clave %d (Correcto, clave duplicada).\n", key);
    }
    if (set_value(key1, v1_set1, N_v2_set1, v2_set1, v3_set1) == 0) {
        printf("Clave %d insertada correctamente.\n", key1);
    } else {
        printf("Error al insertar la clave %d.\n", key1);
    }
    if (set_value(key2, v1_set2, N_v2_set2, v2_set2, v3_set2) == 0) {
        printf("Clave %d insertada correctamente.\n", key2);
    } else {
        printf("Error al insertar la clave %d.\n", key2);
    }
    if (set_value(key3, v1_set3, N_v2_set3, v2_set3, v3_set3) == 0) {
        printf("Clave %d insertada correctamente.\n", key3);
    } else {
        printf("Error al insertar la clave %d.\n", key3);
    }
    if (set_value(key4, v1_set4, N_v2_set4, v2_set4, v3_set4) == 0) {
        printf("Clave %d insertada correctamente (Error, N_value2 fuera de rango).\n", key4);
    } else {
        printf("Error al insertar la clave %d (Correcto, N_value2 fuera de rango).\n", key4);
    }
    if (set_value(key5, v1_set5, N_v2_set5, v2_set5, v3_set5) == 0) {
        printf("Clave %d insertada correctamente (Error, value1 fuera de rango).\n", key5);
    } else {
        printf("Error al insertar la clave %d (Correcto, value1 fuera de rango).\n", key5);
    }

    printf("\n** Pruebas: exist(key) **\n");
    int exists2 = exist(key);
    if (exists2 == 0) {
        printf("La clave %d no existe (Error).\n", key);
    } else {
        printf("La clave %d existe (Correcto).\n", key);
    }
    int exists3 = exist(5);
    if (exists3 == 0) {
        printf("La clave 5 no existe (Correcto).\n");
    } else {
        printf("La clave 5 existe (Error).\n");
    }

    int exists4 = exist(key1);
    if (exists4 == 0) {
        printf("La clave %d no existe (Error).\n", key1);
    } else {
        printf("La clave %d existe (Correcto).\n", key1);
    }

    int exists5 = exist(10);
    if (exists5 == 0) {
        printf("La clave 10 no existe (Correcto).\n");
    } else {
        printf("La clave 10 existe (Error).\n");
    }

    int exists6 = exist(key2);
    if (exists6 == 0) {
        printf("La clave %d no existe (Error).\n", key2);
    } else {
        printf("La clave %d existe (Correcto).\n", key2);
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

    if (get_value(10, v1_get2, &N_v2_get2, v2_get2, &v3_get2) == 0) {
        printf("Clave 10 recuperada correctamente (Error, la clave  no existe).\n");
        printf("   - value1: %s\n", v1_get2);
        printf("   - N_value2: %d\n", N_v2_get2);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get2; i++) {
            printf("%lf ", v2_get2[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get2.x, v3_get2.y);
    } else {
        printf("Error al recuperar la clave (Correcto, la clave no existe).\n");
    }

    if (get_value(key1, v1_get1, &N_v2_get1, v2_get1, &v3_get1) == 0) {
        printf("Clave %d recuperada correctamente.\n", key1);
        printf("   - value1: %s\n", v1_get1);
        printf("   - N_value2: %d\n", N_v2_get1);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get1; i++) {
            printf("%lf ", v2_get1[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get1.x, v3_get1.y);
    } else {
        printf("Error al recuperar la clave %d.\n", key1);
    }

    // 6. Modificar los valores de la clave
    printf("\n** Prueba: modify_value con diferentes claves **\n");
    char v1_mod[] = "valor_modificado";
    double v2_mod[] = {1.1, 3.3, 5.5, 7.7};
    int N_v2_mod = 4;
    struct Coord v3_mod = {20, 10};

    char v1_mod1[] = "modificado_1";
    double v2_mod1[] = {5.5, 10.1};
    int N_v2_mod1 = 2;
    struct Coord v3_mod1 = {12, 8};

    char v1_mod2[] = "modificado_2";
    double v2_mod2[] = {3.14, 6.28, 9.42};
    int N_v2_mod2 = 3;
    struct Coord v3_mod2 = {40, 20};

    if (modify_value(key, v1_mod, N_v2_mod, v2_mod, v3_mod) == 0) {
        printf("Clave %d modificada correctamente.\n", key);
    } else {
        printf("Error al modificar la clave %d.\n", key);
    }

    if (modify_value(20, v1_mod1, N_v2_mod1, v2_mod1, v3_mod1) == 0) {
        printf("Clave 20 modificada correctamente (Error, la clave no exsite).\n");
    } else {
        printf("No se ha podido modificar la clave 20 (Correcto, la clave no exsite) .\n");
    }

    if (modify_value(key2, v1_mod2, N_v2_mod2, v2_mod2, v3_mod2) == 0) {
        printf("Clave %d modificada correctamente.\n", key2);
    } else {
        printf("Error al modificar la clave %d.\n", key2);
    }
    if (modify_value(key, v1_set4, N_v2_set4, v2_set4, v3_set4) == 0) {
        printf("Clave %d modificada correctamente (Error, N_value2 fuera de rango).\n", key);
    } else {
        printf("Error al modificar la clave %d (Correcto, N_value2 fuera de rango).\n", key);
    }
    if (modify_value(key2, v1_set5, N_v2_set5, v2_set5, v3_set5) == 0) {
        printf("Clave %d modificada correctamente (Error, value1 fuera de rango).\n", key2);
    } else {
        printf("Error al modificar la clave %d (Correcto, value1 fuera de rango).\n", key2);
    }

    // 7. Obtener los valores después de modificar
    printf("\n** Prueba: get_value(key) después de modificar **\n");
    if (get_value(key, v1_get4, &N_v2_get4, v2_get4, &v3_get4) == 0) {
        printf("Clave %d recuperada después de modificar.\n", key);
        printf("   - value1: %s\n", v1_get4);
        printf("   - N_value2: %d\n", N_v2_get4);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get4; i++) {
            printf("%lf ", v2_get4[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get4.x, v3_get4.y);
    } else {
        printf("Error al recuperar la clave %d después de modificar.\n", key);
    }

    if (get_value(key2, v1_get3, &N_v2_get3, v2_get3, &v3_get3) == 0) {
        printf("Clave %d recuperada después de modificar.\n", key2);
        printf("   - value1: %s\n", v1_get3);
        printf("   - N_value2: %d\n", N_v2_get3);
        printf("   - V_value2: ");
        for (int i = 0; i < N_v2_get3; i++) {
            printf("%lf ", v2_get3[i]);
        }
        printf("\n   - value3: (%d, %d)\n", v3_get3.x, v3_get3.y);
    } else {
        printf("Error al recuperar la clave %d después de modificar.\n", key2);
    }

    // 8. Eliminar la clave
    printf("\n** Prueba: delete_key(key) **\n");
    if (delete_key(key) == 0) {
        printf("Clave %d eliminada correctamente.\n", key);
    } else {
        printf("Error al eliminar la clave %d.\n", key);
    }
    if (delete_key(20) == 0) {
        printf("Clave 20 eliminada correctamente (Error, la clave no existe).\n");
    } else {
        printf("Error al eliminar la clave 20 (Correcto, la clave no existe).\n");
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
