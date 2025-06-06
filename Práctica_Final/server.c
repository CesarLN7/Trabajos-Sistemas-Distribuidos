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

#include "logic/logic.h"
#include "sockets/sockets.h"

#define MAX_CLIENTS 10

pthread_mutex_t mutex_socket;
int socket_ocupado = false;
pthread_cond_t cond_socket;

int ss = 54321;

// Función para cerrar el servidor
void stop_server() {
    pthread_mutex_destroy(&mutex_socket);
    printf("Cerrando servidor...\n");
    if (ss != 54321) { close(ss); }
    exit(0);
}

// Función para tratar la petición del cliente
void tratar_peticion(void *sc) {
    int s_local;
    char buffer_local[MAXSTR];
    pthread_mutex_lock(&mutex_socket);
    s_local = (*(int *)sc);
    free(sc);
    socket_ocupado = false;
    pthread_cond_signal(&cond_socket);
    pthread_mutex_unlock(&mutex_socket);

    readLine(s_local, buffer_local, MAXSTR);

    // El servidor recibe el comando REGISTER y lo procesa
    if (strcmp(buffer_local, "REGISTER") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        printf("OPERATION REGISTER FROM %s  %s\n", user, fecha_hora);

        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        getpeername(s_local, (struct sockaddr *)&addr, &len);
        char *ip = inet_ntoa(addr.sin_addr);

        int port = 0; // Por ahora 0 (lo real se registra con CONNECT)
        int res = registerUser(user, ip, port);

        char code = (char) ((res == 0) ? 0 : (res == 1) ? 1 : 2);
        send_message(s_local, &code, 1);
    }

    // El servidor recibe el comando UNREGISTER y lo procesa
    else if (strcmp(buffer_local, "UNREGISTER") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        printf("OPERATION UNREGISTER FROM %s  %s\n", user, fecha_hora);

        int res = unregisterUser(user);
        char code = (char)((res == 0) ? 0 : 1);
        send_message(s_local, &code, 1);
    }

    // El servidor recibe el comando CONNECT y lo procesa
    else if (strcmp(buffer_local, "CONNECT") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        char port_str[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        readLine(s_local, port_str, MAXSTR);
        int port = atoi(port_str);
        printf("OPERATION CONNECT FROM %s  %s\n", user, fecha_hora);
    
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        getpeername(s_local, (struct sockaddr *)&addr, &len);
        char *ip = inet_ntoa(addr.sin_addr);
    
        int reg = exist(user);
        if (!reg) {
            char code = 1; // USER DOES NOT EXIST
            send_message(s_local, &code, 1);
        } else {
            int conn = connectUser(user);
            if (conn == 1) { // ya estaba conectado
                char code = 2;
                send_message(s_local, &code, 1);
            } else if (conn == 0) {
                // Actualiza IP y puerto
                updateUserIPPort(user, ip, port);
                char code = 0;
                send_message(s_local, &code, 1);
            } else {
                char code = 3;
                send_message(s_local, &code, 1);
            }
        }
    }
    
    // El servidor recibe el comando DISCONNECT y lo procesa
    else if (strcmp(buffer_local, "DISCONNECT") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        printf("OPERATION DISCONNECT FROM %s  %s\n", user, fecha_hora);

        int res = disconnectUser(user);
        char code = (res == 0) ? 0 : (res == 1 ? 2 : 1); // OK / NOT CONNECTED / USER NOT FOUND
        send_message(s_local, &code, 1);
    }

    // El servidor recibe el comando PUBLISH y lo procesa
    else if (strcmp(buffer_local, "PUBLISH") == 0) {
        char user[MAXSTR], file[MAXSTR], desc[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        readLine(s_local, file, MAXSTR);
        readLine(s_local, desc, MAXSTR);
        printf("OPERATION PUBLISH FROM %s  %s\n", user, fecha_hora);

        if (!exist(user)) {
            char code = 1;  // Usuario no registrado
            send_message(s_local, &code, 1);
        }
        else {
            int res = publishContent(user, file, desc);
            char code = (res == 0) ? 0 : (res == 2 ? 2 : (res == 3 ? 3 : 4)); // OK, no user connected, ya publicado, error
            send_message(s_local, &code, 1);
        }
    }

    // El servidor recibe el comando DELETE y lo procesa
    else if (strcmp(buffer_local, "DELETE") == 0) {
        char user[MAXSTR], file[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        readLine(s_local, file, MAXSTR);
        printf("OPERATION DELETE FROM %s  %s\n", user, fecha_hora);

        if (!exist(user)) {
            char code = 1;  // Usuario no registrado
            send_message(s_local, &code, 1);
        }
        else {
            int res = deleteContent(user, file);
            char code = (res == 0) ? 0 : (res == 2 ? 2 : (res == 3 ? 3 : 4)); // OK, no user connected, ya publicado, error
            send_message(s_local, &code, 1);
        }
    }

    // El servidor recibe el comando LIST_USERS y lo procesa
    else if (strcmp(buffer_local, "LIST_USERS") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        printf("OPERATION LIST_USERS FROM %s  %s\n", user, fecha_hora);

        FILE *fp = fopen("users.txt", "r");
        if (!fp) {
            char code = 2; // error al abrir archivo
            send_message(s_local, &code, 1);
        } else {
            char code = 0;
            send_message(s_local, &code, 1);

            // Contar usuarios
            int count = 0;
            char name[MAXSTR], ip[32];
            int port, connected;
            while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4)
                count++;
            rewind(fp);

            char count_str[MAXSTR];
            sprintf(count_str, "%d", count);
            send_message(s_local, count_str, strlen(count_str) + 1);

            // Enviar los usuarios
            while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4) {
                send_message(s_local, name, strlen(name) + 1);
                send_message(s_local, ip, strlen(ip) + 1);
                sprintf(buffer_local, "%d", port);
                send_message(s_local, buffer_local, strlen(buffer_local) + 1);
            }

            fclose(fp);
        }
    }

    // El servidor recibe el comando LIST_CONTENT y lo procesa
    else if (strcmp(buffer_local, "LIST_CONTENT") == 0) {
        char user[MAXSTR];
        char target_user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        readLine(s_local, target_user, MAXSTR);
        printf("OPERATION LIST_CONTENT FROM %s  %s\n", user, fecha_hora);
    
        if (!exist(user)) {
            char code = 1; // usuario no existe
            send_message(s_local, &code, 1);
        }

        else if (!exist(target_user)) {
            char code = 1; // usuario no existe
            send_message(s_local, &code, 1);

        } else {
            char fname[MAXSTR + 64];
            sprintf(fname, "contents_%s.txt", target_user);
            FILE *fp = fopen(fname, "r");
            if (!fp) {
                char code = 2; // no tiene contenidos
                send_message(s_local, &code, 1);
            } else {
                char code = 0;
                send_message(s_local, &code, 1);
    
                int count = 0;
                while (fgets(buffer_local, MAXSTR, fp)) count++;
                rewind(fp);
    
                char count_str[MAXSTR];
                sprintf(count_str, "%d", count);
                send_message(s_local, count_str, strlen(count_str) + 1);
    
                while (fgets(buffer_local, MAXSTR, fp)) {
                    buffer_local[strcspn(buffer_local, "\n")] = 0;  // Elimina salto de línea
                    char *fichero = strtok(buffer_local, " ");      // Obtiene solo la primera palabra
                    if (fichero != NULL) {
                        send_message(s_local, fichero, strlen(fichero) + 1);
                    }
                }

                fclose(fp);
            }
        }
    }

    // El servidor recibe el comando GET_FILE y lo procesa
    else if (strcmp(buffer_local, "GET_FILE") == 0) {
        char user[MAXSTR];
        char fecha_hora[MAXSTR];
        readLine(s_local, fecha_hora, MAXSTR);
        readLine(s_local, user, MAXSTR);
        printf("OPERATION GET_FILE FROM %s  %s\n", user, fecha_hora);

        FILE *fp = fopen("users.txt", "r");
        if (!fp) {
            char code = 2; // error al abrir archivo
            send_message(s_local, &code, 1);
        } else {
            char code = 0;
            send_message(s_local, &code, 1);

            // Contar usuarios
            int count = 0;
            char name[MAXSTR], ip[32];
            int port, connected;
            while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4)
                count++;
            rewind(fp);

            char count_str[MAXSTR];
            sprintf(count_str, "%d", count);
            send_message(s_local, count_str, strlen(count_str) + 1);

            // Enviar los usuarios
            while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4) {
                send_message(s_local, name, strlen(name) + 1);
                send_message(s_local, ip, strlen(ip) + 1);
                sprintf(buffer_local, "%d", port);
                send_message(s_local, buffer_local, strlen(buffer_local) + 1);
            }

            fclose(fp);
        
        }
    }
    else {
        printf("Comando no reconocido: %s\n", buffer_local);
        char code = 9;
        send_message(s_local, &code, 1);
    }
    
    // Cerrar el socket del cliente y liberar la memoria
    printf("s> ");
    fflush(stdout);
    close(s_local);
    pthread_exit(0);
}

// Función principal del servidor
int main(int argc, char *argv[]) {

    int sc; /* socket del cliente */
    struct sockaddr_in server_addr; /* dirección del servidor */
    int ss, ret; /* socket del servidor */
    int val = 1; /* valor para setsockopt */

    signal(SIGINT, stop_server);
    if (argc != 3) {
        perror("Uso: ./servidor -p <puerto>");
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
    server_addr.sin_port = htons(atoi(argv[2]));

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

    printf("s> init server %d:%d\n", ntohs(server_addr.sin_addr.s_addr), ntohs(server_addr.sin_port));
    printf("s> ");
    fflush(stdout);

    // Bucle infinito para aceptar conexiones
    while (true) {
        struct sockaddr_in client_addr; /* dirección del cliente */
        socklen_t size; /* tamaño de la dirección del cliente */

        size = sizeof(client_addr);
        // poner un trace
        sc = accept(ss, (struct sockaddr*)&client_addr, (socklen_t*)&size);

        if (sc < 0) {
            printf("Error en la aceptación de peticiones del servidor\n");
            continue;
        }

        pthread_t thid; /* identificador del thread */
        pthread_attr_t t_attr; /* atributos del thread */

        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
        int *sc_ptr = malloc(sizeof(int));
        if (sc_ptr == NULL) {
            perror("Error al asignar memoria");
            continue;
        }
        *sc_ptr = sc;
        pthread_create(&thid, &t_attr, (void*)tratar_peticion, (void*)sc_ptr);
        pthread_mutex_lock(&mutex_socket);
        while (socket_ocupado == true) {
            printf("Esperando a que se libere el socket...\n");
            pthread_cond_wait(&cond_socket, &mutex_socket);
        }
        socket_ocupado = true;
        pthread_mutex_unlock(&mutex_socket);
    }
    close(ss);
    printf("The End.\n");
    return 0;
} /*fin main */