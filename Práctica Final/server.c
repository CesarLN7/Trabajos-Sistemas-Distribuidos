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

const int MAXSTR = 256;
pthread_mutex_t mutex_socket;
int socket_ocupado = false;
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

    char username[MAXSTR];

    char buffer_local[MAXSTR];

    pthread_mutex_lock(&mutex_socket);
    s_local = (* (int *)sc);
    free(sc);
    socket_ocupado = false;
    pthread_cond_signal(&cond_socket);
    pthread_mutex_unlock(&mutex_socket);
    
    
    if (readLine(s_local, buffer_local, MAXSTR + 1) < 0) {
        perror("error: readline (coop)");
        close(s_local);
        pthread_exit(NULL);
        return -1;
    }

    if (readLine(s_local, username, MAXSTR + 1) < 0) {
        perror("error: readline (coop)");
        close(s_local);
        pthread_exit(NULL);
        return -1;
    }
    
    printf("s> Tratando operacion de %s\n", username);

    if (strcmp(buffer_local, "REGISTER") == 0) {
        
        //printf("register %s %s %i\n", buffer_local, ip, port); 

        pthread_mutex_lock(&mutex_socket); // acceso a la estructura
        int result = registerUser(username, ip, port);
        pthread_mutex_unlock(&mutex_socket);
        
        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
            return -1;    
        }


    }
    else if (strcmp(buffer_local, "UNREGISTER") == 0) {
      
        //printf("register %s %s %i\n", buffer_local, ip, port); 

        pthread_mutex_lock(&mutex_socket); // acceso a la estructura
        int result = unregisterUser(username);
        pthread_mutex_unlock(&mutex_socket);
        
        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
            return -1;    
        }


    } else if (strcmp(buffer_local, "CONNECT") == 0) {
        
        char port[ARR_SIZE];

        // lectura número de puerto
        if (readLine(local_sc, port, ARR_SIZE) < 0) {
            perror("error: readline connect (port_number)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        } 
        //printf("connect %s %s %s\n", username, sc_ip, port); 
        int result = 0;

        // acceso a la estructura 
        pthread_mutex_lock(&mutex_hilos);
        int index = searchUser(usuarios, username);

        if (index != -1) {
            if (!usuarios->users[index].conected) {
                usuarios->users[index].conected = 1;
                usuarios->users[index].port = atoi(port);
                result = 0;
                // printf("--> %i\n", usuarios->users[index].port);
                // printf("--> %s\n", usuarios->users[index].ip);
                // printf("connect complete\n");
            }
            else {
                result = 2; // user already conected
            }
        }
        else {
            result = 1; // user does not exists
        }
        pthread_mutex_unlock(&mutex_hilos);

        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
        }

        send_rpc(username, "CONNECT", timestamp);


    }
    else if (strcmp(buffer_local, "DISCONNECT") == 0) {
        
        //printf("disconnect %s\n", buffer_local); 
        int result = 0;

        // acceso a la estructura
        pthread_mutex_lock(&mutex_hilos);
        int index = searchUser(usuarios, username);
        if (index != -1) {
            if (usuarios->users[index].conected) {
                usuarios->users[index].conected = 0;
                result = 0;
            }
            else {
                result = 2; // already disconnected
            }
        }
        else {
            result = 1; // user does not exists 
        }
        pthread_mutex_unlock(&mutex_hilos);

        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
        }
        send_rpc(username, "DISCONNECT", timestamp);

    }
    else if (strcmp(buffer_local, "PUBLISH") == 0) {
        //printf("publish\n"); 
        char fileName[ARR_SIZE];
        char description[ARR_SIZE];

        // leer nombre fichero
        if (readLine(local_sc, fileName, ARR_SIZE) < 0) {
            perror("error: readline connect (fileName)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        }

        // leer descripción fichero
        if (readLine(local_sc, description, ARR_SIZE) < 0) {
            perror("error: readline connect (description)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        }

        // acceso a la estructura
        pthread_mutex_lock(&mutex_hilos);
        int result = addContent(usuarios, username, fileName, description);
        pthread_mutex_unlock(&mutex_hilos);

        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
        }

        // concatena op y fileName
        char cadena[ARR_SIZE];
        sprintf(cadena, "%s %s", "PUBLISH", fileName);
        send_rpc(username, cadena, timestamp);

    }
    else if (strcmp(buffer_local, "DELETE") == 0) {
        char fileName[ARR_SIZE];
        
        // leer nombre fichero
        if (readLine(local_sc, fileName, ARR_SIZE) < 0) {
            perror("error: readline connect (fileName)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        }
        //printf("delete ( %s, %s )\n", username, fileName);

        // acceso a la estructura
        pthread_mutex_lock(&mutex_hilos);
        int index = searchUser(usuarios, username);
        int result = 0;
        if (index != -1) {
            if (usuarios->users[index].conected) {
                // todo ok
                result = removeContent(usuarios, username, fileName); 

            }
            else {
                // usuario no conectado
                result = 2;
            }
        }
        else {
            // usuario no existe
            result = 1;
        }
        pthread_mutex_unlock(&mutex_hilos);

        int8_t to_send_result = (int8_t)result;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
        }

        // concat publish + fileName
        char cadena[ARR_SIZE];
        sprintf(cadena, "%s %s", "PUBLISH", fileName);
        send_rpc(username, cadena, timestamp);

    }
    else if (strcmp(buffer_local, "LIST_USERS") == 0) {
        //printf("list_users\n"); 
        
        int8_t to_send_result = (int8_t)0;
        // envio de resultado => 1 byte
        if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
            perror("error sending ret_val");
            close(local_sc);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&mutex_hilos);
        
        // usuarios conectados
        int num_users = 0;
        for (int i = 0; i < usuarios->size; ++i) {
            if (usuarios->users[i].conected)  num_users++;
        }
        sprintf(buffer_local, "%i", num_users); 
        
        if (writeLine(local_sc, buffer_local) < 0) {
            perror("error: writeline list_users (num_users)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        }
        for (int i = 0; i < usuarios->size; ++i) {
            
            user curr = usuarios->users[i];
            if (curr.conected) {
                sprintf(buffer_local, "%s", curr.name);
                if (writeLine(local_sc, buffer_local) < 0) {
                    perror("error: writeline list_users (name)");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }
                sprintf(buffer_local, "%s", curr.ip);
                if (writeLine(local_sc, buffer_local) < 0) {
                    perror("error: writeline list_users (ip)");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }
                sprintf(buffer_local, "%i", curr.port);
                if (writeLine(local_sc, buffer_local) < 0) {
                    perror("error: writeline list_users (port)");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }
            }
            
        }
        pthread_mutex_unlock(&mutex_hilos);
        //printf("complete list_users\n"); 
        
        send_rpc(username, "LIST_USERS", timestamp); 


    }
    else if (strcmp(buffer_local, "LIST_CONTENT") == 0) {
        
        // leer nombre de usuario (argumento)
        if (readLine(local_sc, buffer_local, ARR_SIZE) < 0) {
            perror("error: readline connect (username)");
            close(local_sc);
            pthread_exit(NULL);
            return -1;
        }
        //printf("list_content %s\n", username);

        // acceso a la estructura
        pthread_mutex_lock(&mutex_hilos);
        int index = searchUser(usuarios, buffer_local);
        if (-1 == index) {
            // user does not exists 
            int8_t to_send_result = (int8_t)1;
            // envio de resultado => 1 byte
            if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
                perror("error sending ret_val");
                close(local_sc);
                pthread_exit(NULL);
                return -1;
            }

        }
        else {
            //printf("listando contenidos:\n");
            if (!usuarios->users[index].conected) {
                int8_t to_send_result = (int8_t)2;
                // envio de resultado => 1 byte
                if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
                    perror("error sending ret_val");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }
            } else {
                int8_t to_send_result = (int8_t)0;
                // envio de resultado => 1 byte
                if (sendMessage(local_sc, (char *)&to_send_result, 1) < 0) {
                    perror("error sending ret_val");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }

                sprintf(buffer_local, "%i", usuarios->users[index].contentsLen); // tamaño de la lista
                if (writeLine(local_sc, buffer_local) < 0) {
                    perror("error: writeLine list_content (Contentslen)");
                    close(local_sc);
                    pthread_exit(NULL);
                    return -1;
                }
                //printf("%s\n", buffer_local);
                for (int i = 0; i < usuarios->users[index].contentsLen; ++i) {
                    //printf("--%s %s\n", usuarios->users[index].contents[i].name, usuarios->users[index].contents[i].description);

                    if (writeLine(local_sc, usuarios->users[index].contents[i].name) < 0) {
                        perror("error: writeLine list_content (FileName)");
                        close(local_sc);
                        pthread_exit(NULL);
                        return -1;
                    }

                    if (writeLine(local_sc, usuarios->users[index].contents[i].description) < 0) {
                        perror("error: writeLine list_content (description)");
                        close(local_sc);
                        pthread_exit(NULL);
                        return -1;
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex_hilos);

        send_rpc(username, "LIST_CONTENT", timestamp); 
        

    }
    else {
        fprintf(stderr, "server: not recognised operation (%s)\n", buffer_local);
        close(local_sc);
        pthread_exit(NULL);
        return -1;
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
        // poner un trace
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