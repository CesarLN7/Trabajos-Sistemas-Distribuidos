#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#include "claves/claves.h"
#include "mensaje.h"
#include "sockets/sockets.h"

#define MAX_CLIENTS 10

pthread_mutex_t mutex_socket;
int socket_libre = true;
pthread_cond_t cond_socket;

int ss = 54321;

void stop_server() {
    pthread_mutex_destroy(&mutex_socket);
    printf("Cerrando servidor...\n");
    if (ss != 54321) { close(ss); }
    exit(0);
}

void tratar_peticion(void *sc) {
    int s_local;

    int op;
    int key;
    char value1[MAXSTR];
    int N_value2;
    double V_value2[MAXVEC];
    int res;
    struct Coord value3;

    char buffer_local[MAXSTR];

    pthread_mutex_lock(&mutex_socket);
    s_local = (* (int *)sc);
    socket_libre = false;
    pthread_cond_signal(&cond_socket);
    pthread_mutex_unlock(&mutex_socket);
    
    printf("tratando peticion de %i...\n", s_local);
    readLine(s_local, buffer_local, sizeof(char) + 1);
    op = atoi(buffer_local);

    switch (op) {
            case 1: // DESTROY
                    res = destroy();
                    sprintf(buffer_local, "%i", res);
                    send_message(s_local, buffer_local, sizeof(buffer_local) + 1);
                    break;
            case 2: // SET VALUE
                    res = set_value(pr.key, pr.value1, pr.N_value2, pr.V_value2, pr.value3); // FALTAN ESTAS TRES FUNCIONES POR AJUSTAR
                    break;
            case 3: // GET VALUE
                    res = get_value(pr.key, pr.value1, &pr.N_value2, pr.V_value2, &pr.value3);
                    break;
            case 4: // MODIFY VALUE
                    res = modify_value(pr.key, pr.value1, pr.N_value2, pr.V_value2, pr.value3); // HASTA AQUÍ
                    break;
            case 5: // DELETE KEY
                    readLine(s_local, buffer_local, MAXSTR);
                    key = atoi(buffer_local);
                    res = delete_key(key);
                    sprintf(buffer_local, "%i", res);
                    send_message(s_local, buffer_local, sizeof(buffer_local) + 1);
                    break;
            case 6: // EXIST
                    readLine(s_local, buffer_local, MAXSTR);
                    key = atoi(buffer_local);
                    res = exist(key);
                    sprintf(buffer_local, "%i", res);
                    send_message(s_local, buffer_local, sizeof(buffer_local) + 1);
                    break;
            default: // Operación desconocida.
                    res = -1;
                    break;
    }

    close(s_local);
    pthread_exit(0);

}

int main(int argc, char *argv[]) {

    int sc; /* socket del cliente */
    struct sockaddr_in server_addr; /* dirección del servidor */
    int ss, ret; /* socket del servidor */
    int val = 1; /* valor para setsockopt */

    signal(SIGINT, stop_server);
    if (argc != 2) {
        perror("Uso: ./servidor <puerto>");
        return -1;
    }

    ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0) {
        printf("SERVER: Error en el socket\n");
        return (0);
    }

    setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(int));

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    ret = bind(ss, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        printf("Error en bind\n");
        return -1;
    }

    ret = listen(ss, MAX_CLIENTS);
    if (ret == -1) {
        printf("Error en listen\n");
        return -1;
    }

    if (ss < 0) {
        printf("SERVER: Error en la inicialización del socket del servidor\n");
        return 0;
    }

    while (true) {
        struct sockaddr_in client_addr; /* dirección del cliente */
        socklen_t size; /* tamaño de la dirección del cliente */

        printf("esperando conexion...\n");

        size = sizeof(client_addr);
        sc = accept(ss, (struct sockaddr*)&client_addr, (socklen_t*)&size);

        printf("conexión aceptada de IP: %s y puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (sc < 0) {
            printf("Error en la aceptación de peticiones del servidor\n");
            continue;
        }

        pthread_t thid; /* identificador del thread */
        pthread_attr_t t_attr; /* atributos del thread */

        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thid, &t_attr, (void*)tratar_peticion, (void*)&sc);
        pthread_mutex_lock(&mutex_socket);
        while (socket_libre == false) {
            printf("Esperando a que se libere el socket...\n");
            pthread_cond_wait(&cond_socket, &mutex_socket);
        }
        socket_libre = true;
        pthread_mutex_unlock(&mutex_socket);
    }
    close (ss);
    printf("The End.\n");
    return 0;
} /*fin main */