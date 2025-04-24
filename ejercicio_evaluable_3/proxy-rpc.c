// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "claves/claves.h"
#include "claves-rpc.h"

// Librerías básicas
#include <fcntl.h>      /* For O_* constants */
#include <sys/stat.h>   /* For mode constants */
#include <sys/types.h>  /* For mode constants */
#include <string.h>     /* For strlen, strcpy, sprintf... */
#include <unistd.h>     /* For sleep */
#include <stdio.h>      /* For printf */
#include <stdlib.h>     /* For exit */


// Error en localhost -> en vez de 127.0.0.1 lo pone a 255.255.255.255
// Esto se usa para una comprobación a futuro, pero el programa usa la variable de entorno
#define localhost "127.0.0.1"

//    ----  VARIABLES GLOBALES  ----    //

// Cliente del RPC
CLIENT *clnt;

// Retorno de las funciones de los RPCs:
enum clnt_stat retval_1;
enum clnt_stat retval_2;
enum clnt_stat retval_3;
enum clnt_stat retval_4;
enum clnt_stat retval_5;
enum clnt_stat retval_6;

// Valores de retorno reales
int result_1;
int result_2;
message result_3; // Resultado del get_value
int result_4;
int result_5;
int result_6;

// Parámetros de las funciones de los RPCs
int get_value_tuplas_1_key;
message set_value_tuplas_1_msg;
message modify_value_tuplas_1_msg;
int delete_key_tuplas_1_key;
int exist_tuplas_1_key;

// ----------     FUNCIONES AUXILIARES    ---------- //

// Función para obtener la variable de entorno IP_TUPLAS
char* get_ip_tuplas() {
    printf("[comms][client] Obteniendo IP_TUPLAS\n");
    // Obtenemos la variable de entorno IP_TUPLAS
	char* ip_tuplas = getenv("IP_TUPLAS");
    // Si no se ha establecido la variable de entorno, se muestra un error
	if (ip_tuplas == NULL) {
		printf("No ha sido establecida IP_TUPLAS\n\n\n");
		exit(1);
	}
    // si no tenemos en el sistema localhost como 127.0.0.1 lo cambiamos con el valor del #define
	if (strcmp(ip_tuplas, "localhost") == 0) {
		strcpy(ip_tuplas, localhost);
    };
    printf("[comms][client] Obtenido IP_TUPLAS: %s\n", ip_tuplas);
	return ip_tuplas;
}

// ----------     FUNCIONES PRINCIPALES    ---------- //
int destroy() {
    // Creamos el rpc y nos conectamos al servidor
    printf("[comms][client] Iniciando la operación init\n");
    char* ip_tuplas = get_ip_tuplas();
	clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
		clnt_pcreateerror (ip_tuplas);
		exit (1);
	};
    retval_1 = init_tuplas_1(&result_1, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_1 != RPC_SUCCESS) {
	    clnt_perror (clnt, "call failed");
    };
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
	return result_1;
};

int set_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación set_value\n");
    char* ip_tuplas = get_ip_tuplas();
    // Creamos el rpc y nos conectamos al servidor
    clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (ip_tuplas);
        exit (1);
    };
    // Copiamos los valores en la estructura
    set_value_tuplas_1_msg.key_or_return = key;
    strcpy(set_value_tuplas_1_msg.value1, value1);
    set_value_tuplas_1_msg.n_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        set_value_tuplas_1_msg.v_value2[i] = V_value_2[i];
    }
    // Llamamos a la función del servidor
    retval_2 = set_value_tuplas_1(set_value_tuplas_1_msg, &result_2, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_2 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    };
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
	return result_2;
}

int get_value(int key, char* value1, int* N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación get_value\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (ip_tuplas);
        exit (1);
    };
    // Llamamos a la función del servidor
    get_value_tuplas_1_key = key;
    retval_3 = get_value_tuplas_1(get_value_tuplas_1_key, &result_3, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_3 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    };
    // Copiamos los valores en las variables de salida
    strcpy(value1, result_3.value1);
    *N_value2 = result_3.n_value2;
    for (int i = 0; i < *N_value2; i++) {
        V_value_2[i] = result_3.v_value2[i];
    }
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
	return result_3.key_or_return;
}

int modify_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación modify_value\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (ip_tuplas);
        exit (1);
    };
    // Copiamos los valores en la estructura
    modify_value_tuplas_1_msg.key_or_return = key;
    strcpy(modify_value_tuplas_1_msg.value1, value1);
    modify_value_tuplas_1_msg.n_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        modify_value_tuplas_1_msg.v_value2[i] = V_value_2[i];
    }
    // Llamamos a la función del servidor
    retval_4 = modify_value_tuplas_1(modify_value_tuplas_1_msg, &result_4, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_4 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    };
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
    return result_4;
}

int delete_key(int key) {
    printf("[comms][client] Iniciando la operación delete_key\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (ip_tuplas);
        exit (1);
    };
    // Llamamos a la función del servidor
    delete_key_tuplas_1_key = key;
    retval_5 = delete_key_tuplas_1(delete_key_tuplas_1_key, &result_5, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_5 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    };
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
    return result_5;
}

int exist(int key) {
    printf("[comms][client] Iniciando la operación exist\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create (ip_tuplas, TUPLAS, TUPLASVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (ip_tuplas);
        exit (1);
    };
    // Llamamos a la función del servidor
    exist_tuplas_1_key = key;
    retval_6 = exist_tuplas_1(exist_tuplas_1_key, &result_6, clnt);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    if (retval_6 != RPC_SUCCESS) {
        clnt_perror (clnt, "call failed");
    };
    // Cerramos el rpc
    printf("[comms][client] Cerrando el rpc\n\n\n");
    clnt_destroy (clnt);
    return result_6;
}