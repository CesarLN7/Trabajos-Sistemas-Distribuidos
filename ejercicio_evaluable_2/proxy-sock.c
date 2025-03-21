#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "proxy-sock.h"
#include "mensaje.h"
#include "sockets/sockets.h"

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

int connect_to_server() {
    int sock;
    struct sockaddr_in server_addr;
    char *ip = getenv("IP_TUPLAS");
    char *port_str = getenv("PORT_TUPLAS");
    if (!ip || !port_str) {
        fprintf(stderr, "Variables de entorno IP_TUPLAS y PORT_TUPLAS no definidas.\n");
        exit(1);
    }
    int port = atoi(port_str);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error al crear socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        return -1;
    }
    return sock;
}

int send_request(struct message *msg) {
    int sock = connect_to_server();
    if (sock < 0) return -1;

    if (send_message(sock, (char*)msg, sizeof(struct message)) < 0) {
        perror("Error al enviar mensaje");
        close(sock);
        return -1;
    }

    if (receive_message(sock, (char*)msg, sizeof(struct message)) < 0) {
        perror("Error al recibir respuesta");
        close(sock);
        return -1;
    }
    close(sock);
    return msg->res;
}

int destroy() {
    struct message msg = { .op = 1 };
    return send_request(&msg);
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    is_value1_valid(value1);
    is_N_value2_valid(N_value2);
    struct message msg = { .op = 2, .key = key, .N_value2 = N_value2, .value3 = value3 };
    strncpy(msg.value1, value1, MAXSTR);
    memcpy(msg.V_value2, V_value2, sizeof(double) * N_value2);
    return send_request(&msg);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    struct message msg = { .op = 3, .key = key };
    int res = send_request(&msg);
    if (res == 0) {
        strncpy(value1, msg.value1, MAXSTR);
        *N_value2 = msg.N_value2;
        memcpy(V_value2, msg.V_value2, sizeof(double) * (*N_value2));
        *value3 = msg.value3;
    }
    return res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    is_value1_valid(value1);
    is_N_value2_valid(N_value2);
    struct message msg = { .op = 4, .key = key, .N_value2 = N_value2, .value3 = value3 };
    strncpy(msg.value1, value1, MAXSTR);
    memcpy(msg.V_value2, V_value2, sizeof(double) * N_value2);
    return send_request(&msg);
}

int delete_key(int key) {
    struct message msg = { .op = 5, .key = key };
    return send_request(&msg);
}

int exist(int key) {
    struct message msg = { .op = 6, .key = key };
    return send_request(&msg);
}
