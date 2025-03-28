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

// REVISA EL FUNCIONAMIENTO DE AQUI PARA ABAJO TODO
int send_request(char *request, char *response, size_t response_size) {
    int ss = connect_to_server();
    if (ss < 0) return -1;

    send_message(ss, request, strlen(request) + 1);
    readLine(ss, response, response_size);

    close(ss);
    return atoi(response);
}

int destroy() {
    return send_request("1", buffer_local, sizeof(buffer_local));
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    char request[1024];
    snprintf(request, sizeof(request), "2 %d %s %d %.6f %.6f", key, value1, N_value2, value3.x, value3.y);
    
    return send_request(request, buffer_local, sizeof(buffer_local));
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    char request[32];
    snprintf(request, sizeof(request), "3 %d", key);

    char response[1024];
    if (send_request(request, response, sizeof(response)) < 0) return -1;

    sscanf(response, "%s %d %lf %lf", value1, N_value2, &value3->x, &value3->y); // Esto está mal porque V_value2 no lo printea !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (is_value1_valid(value1) == -1 || is_N_value2_valid(N_value2) == -1) {
        return -1;
    }

    char request[1024];
    snprintf(request, sizeof(request), "4 %d %s %d %.6f %.6f", key, value1, N_value2, value3.x, value3.y);

    return send_request(request, buffer_local, sizeof(buffer_local));
}

int delete_key(int key) {
    char request[32];
    snprintf(request, sizeof(request), "5 %d", key);

    return send_request(request, buffer_local, sizeof(buffer_local));
}

int exist(int key) {
    char request[32];
    snprintf(request, sizeof(request), "6 %d", key);
    
    return send_request(request, buffer_local, sizeof(buffer_local));
}
