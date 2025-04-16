#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "proxy-rpc.h"
#include "mensaje.h"

#define SERVER_PORT "tcp"

CLIENT *clnt = NULL;

// función para comprobar si la longitud de value1 es correcta
int is_value1_valid(char *value1) {
    if (strlen(value1) > MAXSTR) {
        printf("Error: La cadena excede los 255 caracteres permitidos.\n");
        return -1;
    }
}

// función para comprobar si N_value2 está en el rango correspondiente
int is_N_value2_valid(int N_value2) {
    if (N_value2 < 1 || N_value2 > MAXVEC) {
        printf("Error: El valor de N_value2 no es correcto.\n");
        return -1; // Error: N_value2 fuera de rango
    }
}

// Función para conseguir la ip del servidor
char* get_ip() {
    char* ip = getenv("IP_TUPLAS");
    if (ip == NULL) {
        perror("NOT setted ip");
        return NULL;
    }
    return ip;
}

// Conexión con el servidor
int connect_to_server() {
    if (clnt == NULL) {
        char* ip = get_ip();
        if (ip == NULL) return -1;

        clnt = clnt_create(ip, INTERFAZ, FUNCIONESVER, SERVER_PORT);
        if (clnt == NULL) {
            clnt_pcreateerror("Error al crear el cliente RPC");
            return -1;
        }
    }
    return 0;
}

void disconnect_from_server() {
    if (clnt != NULL) {
        clnt_destroy(clnt);
        clnt = NULL;
    }
}

int destroy() {
    if (connect_to_server() < 0) return -1;

    int result;
    if (rpc_destroy_1(&result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en destroy()");
        return -1;
    }

    return result;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    if (connect_to_server() < 0) return -1;

    struct args_in args;
    strncpy(args.value1, value1, MAXSTR);
    args.N_value2 = N_value2;
    memcpy(args.V_value2.V_value2_val, V_value2, sizeof(double) * N_value2);
    args.value3 = value3;

    int result;
    if (rpc_set_value_1(key, args, &result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en set_value()");
        return -1;
    }

    return result;

}


int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    if (connect_to_server() < 0) return -1;

    struct args_out result;
    memset(&result, 0, sizeof(result));

    if (rpc_get_value_1(key, &result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en get_value()");
        return -1;
    }

    strncpy(value1, result.value1, MAXSTR);
    *N_value2 = result.N_value2;
    memcpy(V_value2, result.V_value2, sizeof(double) * (*N_value2));
    *value3 = result.value3;

    return result.res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    if (connect_to_server() < 0) return -1;

    struct args_in args;
    strncpy(args.value1, value1, MAXSTR);
    args.N_value2 = N_value2;
    memcpy(args.V_value2, V_value2, sizeof(double) * N_value2);
    args.value3 = value3;

    int result;
    if (rpc_modify_value_1(key, args, &result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en modify_value()");
        return -1;
    }
    return result;

}

int delete_key(int key) {
    if (connect_to_server() < 0) return -1;

    int result;
    if (rpc_delete_key_1(key, &result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en delete_key()");
        return -1;
    }
    return result;
}

int exist(int key) {
    if (connect_to_server() < 0) return -1;

    int result;
    if (rpc_exist_1(key, &result, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error en exist()");
        return -1;
    }
    return result;
}
