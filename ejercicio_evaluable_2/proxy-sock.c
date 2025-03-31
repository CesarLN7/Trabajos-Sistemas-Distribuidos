#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "proxy-sock.h"
#include "mensaje.h"
#include "sockets/sockets.h"

char buffer_local[MAXSTR]; // buffer para almacenar mensajes

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

// Función para conectar con el servidor
int connect_to_server() {
    int ss;
    struct sockaddr_in server_addr;
    char *ip = getenv("IP_TUPLAS");
    char *port_str = getenv("PORT_TUPLAS");
    if (!ip || !port_str) {
        fprintf(stderr, "Variables de entorno IP_TUPLAS y PORT_TUPLAS no definidas.\n");
        return -1;
    }
    int port = atoi(port_str);

    ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0) {
        perror("Error al crear socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(ss, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        return -1;
    }
    return ss;
}

int destroy() {
    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "1"; // Mandamos así el código de operación para que incluya el '\0' al final de la cadena
    send_message(ss, op, 2);

    char res[MAXSTR];
    readLine(ss, res, MAXSTR);
    return atoi(res);

}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "2";
    send_message(ss, op, 2);

    // Enviamos la clave al servidor
    char entero_a_cadena[MAXSTR];
    sprintf(entero_a_cadena, "%d", key);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    // Enviamos value1 al servidor
    send_message(ss, value1, sizeof(value1) + 1);

    // Enviamos N_value2 al servidor
    sprintf(entero_a_cadena, "%d", N_value2);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    // Enviamos V_value2 al servidor
    for (int i = 0; i < N_value2; i++) {
        sprintf(entero_a_cadena, "%lf", V_value2[i]);
        send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);
    }

    // Enviamos value3 al servidor
    sprintf(entero_a_cadena, "%d", value3.x);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);
    sprintf(entero_a_cadena, "%d", value3.y);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    char res[MAXSTR];
    readLine(ss, res, MAXSTR);
    return atoi(res);

}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "3";
    send_message(ss, op, 2);

    // Enviamos la clave al servidor
    char entero_a_cadena[MAXSTR];
    sprintf(entero_a_cadena, "%d", key);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    // Leemos value1 del servidor
    readLine(ss, entero_a_cadena, MAXSTR);
    strcpy(value1, entero_a_cadena);

    // Leemos N_value2 del servidor
    readLine(ss, entero_a_cadena, MAXSTR);
    *N_value2 = atoi(entero_a_cadena);

    // Leemos V_value2 del servidor
    for (int i = 0; i < *N_value2; i++) {
        readLine(ss, entero_a_cadena, MAXSTR);
        V_value2[i] = atof(entero_a_cadena);
    }

    // Leemos value3 del servidor
    readLine(ss, entero_a_cadena, MAXSTR);
    value3->x = atoi(entero_a_cadena);
    readLine(ss, entero_a_cadena, MAXSTR);
    value3->y = atoi(entero_a_cadena);

    // Leemos el resultado del servidor
    char res[MAXSTR];
    readLine(ss, res, MAXSTR);
    return atoi(res);

}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "4";
    send_message(ss, op, 2);

    // Enviamos la clave al servidor
    char entero_a_cadena[MAXSTR];
    sprintf(entero_a_cadena, "%d", key);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    // Enviamos value1 al servidor
    send_message(ss, value1, sizeof(value1) + 1);

    // Enviamos N_value2 al servidor
    sprintf(entero_a_cadena, "%d", N_value2);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    // Enviamos V_value2 al servidor
    for (int i = 0; i < N_value2; i++) {
        sprintf(entero_a_cadena, "%lf", V_value2[i]);
        send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);
    }

    // Enviamos value3 al servidor
    sprintf(entero_a_cadena, "%d", value3.x);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);
    sprintf(entero_a_cadena, "%d", value3.y);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    char res[MAXSTR];
    readLine(ss, res, MAXSTR);
    return atoi(res);
    
}

int delete_key(int key) {
    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "5";
    send_message(ss, op, 2);
    
    // Enviamos la clave al servidor
    char entero_a_cadena[MAXSTR];
    sprintf(entero_a_cadena, "%d", key);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);

    char res[MAXSTR];
    readLine(ss, res, MAXSTR);
    return atoi(res);
}

int exist(int key) {
    int ss = connect_to_server();
    if (ss < 0) return -1;

    char op[] = "6"; 
    send_message(ss, op, 2);

    // Enviamos la clave al servidor
    char entero_a_cadena[MAXSTR];
    sprintf(entero_a_cadena, "%d", key);
    send_message(ss, entero_a_cadena, sizeof(entero_a_cadena) + 1);
    
    char res[MAXSTR];
    readLine(ss, res, MAXSTR);

    return atoi(res);
}
