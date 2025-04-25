// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "claves/claves.h"
#include "claves-rpc.h"
#include "mensaje.h"

// Librerías básicas
#include <fcntl.h>      
#include <sys/stat.h>   
#include <sys/types.h>  
#include <string.h>     
#include <unistd.h>     
#include <stdio.h>      
#include <stdlib.h>    


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
args_struct result_3; // Resultado del get_value
int result_4;
int result_5;
int result_6;

// Parámetros de las funciones de los RPCs
int rpc_get_value_1_key;
args_struct rpc_set_value_1_aux;
args_struct rpc_modify_value_1_aux;
int rpc_delete_key_1_key;
int rpc_exist_1_key;

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
// Función para obtener la variable de entorno IP_TUPLAS
char* get_ip_tuplas() {
    printf("Obteniendo IP_TUPLAS\n");
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
    printf("Obtenido IP_TUPLAS: %s\n", ip_tuplas);
	return ip_tuplas;
}


int destroy() {
    // Creamos el rpc y nos conectamos al servidor
    printf("Iniciando destroy\n");
    char* ip_tuplas = get_ip_tuplas();
	clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
		clnt_pcreateerror(ip_tuplas);
		exit(1);
	};
    retval_1 = rpc_destroy_1(&result_1, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_1 != RPC_SUCCESS) {
	    clnt_perror(clnt, "Error RPC");
    };
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
	return result_1;
};

int set_value(int key, char* value1, int N_value2, double* V_value_2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }
    printf("Iniciando set_value\n");
    char* ip_tuplas = get_ip_tuplas();
    // Creamos el rpc y nos conectamos al servidor
    clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip_tuplas);
        exit(1);
    };
    // Copiamos los valores en la estructura
    rpc_set_value_1_aux.key = key;
    strcpy(rpc_set_value_1_aux.value1, value1);
    rpc_set_value_1_aux.N_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        rpc_set_value_1_aux.V_value2[i] = V_value_2[i];
    }
    rpc_set_value_1_aux.value3.x = value3.x;
	rpc_set_value_1_aux.value3.y = value3.y;
    // Llamamos a la función del servidor
    retval_2 = rpc_set_value_1(rpc_set_value_1_aux, &result_2, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_2 != RPC_SUCCESS) {
        clnt_perror(clnt, "Error RPC");
    };
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
	return result_2;
}

int get_value(int key, char* value1, int* N_value2, double* V_value_2, struct Coord *value3) {
    printf("Iniciando get_value\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip_tuplas);
        exit(1);
    };
    // Llamamos a la función del servidor
    rpc_get_value_1_key = key;
    retval_3 = rpc_get_value_1(rpc_get_value_1_key, &result_3, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_3 != RPC_SUCCESS) {
        clnt_perror(clnt, "Error RPC");
    };
    // Copiamos los valores en las variables de salida
    strcpy(value1, result_3.value1);
    *N_value2 = result_3.N_value2;
    for (int i = 0; i < *N_value2; i++) {
        V_value_2[i] = result_3.V_value2[i];
    }
    value3->x = result_3.value3.x;
    value3->y = result_3.value3.y;
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
	return result_3.key;
}

int modify_value(int key, char* value1, int N_value2, double* V_value_2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    printf("Iniciando modify_value\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip_tuplas);
        exit(1);
    };
    // Copiamos los valores en la estructura
    rpc_modify_value_1_aux.key = key;
    strcpy(rpc_modify_value_1_aux.value1, value1);
    rpc_modify_value_1_aux.N_value2 = N_value2;
    for (int i = 0; i < N_value2; i++) {
        rpc_modify_value_1_aux.V_value2[i] = V_value_2[i];
    }
    rpc_modify_value_1_aux.value3.x = value3.x;
	rpc_modify_value_1_aux.value3.y = value3.y;
    // Llamamos a la función del servidor
    retval_4 = rpc_modify_value_1(rpc_modify_value_1_aux, &result_4, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_4 != RPC_SUCCESS) {
        clnt_perror(clnt, "Error RPC");
    };
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
    return result_4;
}

int delete_key(int key) {
    printf("Iniciando delete_key\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip_tuplas);
        exit(1);
    };
    // Llamamos a la función del servidor
    rpc_delete_key_1_key = key;
    retval_5 = rpc_delete_key_1(rpc_delete_key_1_key, &result_5, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_5 != RPC_SUCCESS) {
        clnt_perror(clnt, "Error RPC");
    };
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
    return result_5;
}

int exist(int key) {
    printf("Iniciando exist\n");
    // Creamos el rpc y nos conectamos al servidor
    char* ip_tuplas = get_ip_tuplas();
    clnt = clnt_create(ip_tuplas, INTERFAZ, FUNCIONESVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip_tuplas);
        exit(1);
    };
    // Llamamos a la función del servidor
    rpc_exist_1_key = key;
    retval_6 = rpc_exist_1(rpc_exist_1_key, &result_6, clnt);
    printf("Se ha conectado al servidor\n");
    if (retval_6 != RPC_SUCCESS) {
        clnt_perror(clnt, "Error RPC");
    };
    // Cerramos el rpc
    printf("Cerrando el RPC\n\n\n");
    clnt_destroy(clnt);
    return result_6;
}