#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "claves/claves.h"
#include "mensaje.h"
#include "sockets/sockets.h"

#define MAX_CLIENTS 10

int server_socket;

void sigHandler(int signo) {
    close(server_socket);
    printf("\nServidor cerrado.\n");
    exit(0);
}

void handle_client(int client_socket) {
    struct message msg;
    while (receive_message(client_socket, (char*)&msg, sizeof(struct message)) == 0) {
        switch (msg.op) {
            case 1:
                msg.res = destroy();
                break;
            case 2:
                msg.res = set_value(msg.key, msg.value1, msg.N_value2, msg.V_value2, msg.value3);
                break;
            case 3:
                msg.res = get_value(msg.key, msg.value1, &msg.N_value2, msg.V_value2, &msg.value3);
                break;
            case 4:
                msg.res = modify_value(msg.key, msg.value1, msg.N_value2, msg.V_value2, msg.value3);
                break;
            case 5:
                msg.res = delete_key(msg.key);
                break;
            case 6:
                msg.res = exist(msg.key);
                break;
            default:
                msg.res = -1;
                break;
        }
        send_message(client_socket, (char*)&msg, sizeof(struct message));
    }
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PUERTO>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    signal(SIGINT, sigHandler);
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        return -1;
    }

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error en listen");
        return -1;
    }

    printf("Servidor escuchando en el puerto %d...\n", port);
    
    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Error en accept");
            continue;
        }
        if (!fork()) {
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        }
        close(client_socket);
    }
    return 0;
}
