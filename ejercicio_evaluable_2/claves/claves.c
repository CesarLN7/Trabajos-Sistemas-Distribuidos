#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "claves.h"
#include "../mensaje.h"

int destroy() {
    FILE *file = fopen("data.txt", "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return -1; // Error
    }
    fclose(file);
    printf("Tuplas borradas\n");
    return 0; // Éxito
}

int exist(int key) {
    FILE *f = fopen("data.txt", "r");
    if (f == NULL) {
        return 0; // No existe
    }
    
    int k;
    char v1[MAXSTR];
    int N_v2;
    double V_v2[MAXVEC];
    struct Coord value3;
    
    while (fscanf(f, "%d %s %d", &k, v1, &N_v2) != EOF) {
        for (int i = 0; i < N_v2; i++) {
            fscanf(f, "%lf", &V_v2[i]);
        }
        fscanf(f, "%d %d", &value3.x, &value3.y);
        
        if (k == key) {
            fclose(f);
            printf("La clave %d existe\n", key);
            return 1; // Existe
        }
    }
    fclose(f);
    printf("La clave %d no existe\n", key);
    return 0; // No existe
}


int delete_key(int key) {
    if (!exist(key)) {
        return -1; // Error: clave no existe
    }
    
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        return -1; // Error al abrir el archivo
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(file);
        return -1; // Error al crear el archivo temporal
    }
    
    int k;
    char v1[MAXSTR];
    int N_v2;
    double V_v2[MAXVEC];
    struct Coord value3;
    
    while (fscanf(file, "%d %s %d", &k, v1, &N_v2) != EOF) {
        for (int i = 0; i < N_v2; i++) {
            fscanf(file, "%lf", &V_v2[i]);
        }
        fscanf(file, "%d %d", &value3.x, &value3.y);
        
        if (k != key) {
            fprintf(temp, "%d %s %d ", k, v1, N_v2);
            for (int i = 0; i < N_v2; i++) {
                fprintf(temp, "%lf ", V_v2[i]);
            }
            fprintf(temp, "%d %d\n", value3.x, value3.y);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    remove("data.txt");
    rename("temp.txt", "data.txt");
    printf("La clave %d se ha borrado correctamente\n", key);
    return 0; // Éxito
}


int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    printf("La clave %d se va a insertar\n", key);

    if (exist(key)) {
        return -1; // Error: clave ya existe
    }
    
    FILE *file = fopen("data.txt", "a");
    if (file == NULL) {
        return -1; // Error al abrir el archivo
    }
    
    fprintf(file, "%d %s %d ", key, value1, N_value2);
    for (int i = 0; i < N_value2; i++) {
        fprintf(file, "%lf ", V_value2[i]);
    }
    fprintf(file, "%d %d\n", value3.x, value3.y);
    fclose(file);
    printf("La clave %d se ha insertado correctamente\n", key);

    return 0; // Éxito
}


int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        return -1; // Error al abrir el archivo
    }
    
    int k;
    while (fscanf(file, "%d %s %d", &k, value1, N_value2) != EOF) {
        for (int i = 0; i < *N_value2; i++) {
            fscanf(file, "%lf", &V_value2[i]);
        }
        fscanf(file, "%d %d", &value3->x, &value3->y);
        
        if (k == key) {
            fclose(file);
            printf("La clave %d se ha obtenido correctamente\n", key);
            return 0; // Éxito
        }
    }
    
    fclose(file);
    return -1; // Error: clave no encontrada
}


int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    printf("La clave %d se va a modificar\n", key);
    int exists = exist(key);
    if (exists == 0)
        return -1;
    // Borramos la key y la insertamos de nuevo con los nuevos valores
    delete_key(key);
    set_value(key, value1, N_value2, V_value2, value3);
    printf("La clave %d se ha modificado con éxito\n", key);    
    return 0;
}