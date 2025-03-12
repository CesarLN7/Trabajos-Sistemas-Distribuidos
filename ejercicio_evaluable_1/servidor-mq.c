#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <string.h>
 
#include "claves/claves.h"
#include "mensaje.h"
 
#define SERVER_QUEUE "/SERVIDOR"   // Nombre de la cola del servidor
 
int do_exit = 0;  // Flag para terminar el servidor
 
// Manejador de SIGINT (Ctrl+C) para finalizar el servidor de forma segura.
void sigHandler(int signo) {
        do_exit = 1;
}
 
// Función para tratar cada petición recibida.
// Dependiendo del campo 'op', se invoca la función correspondiente de la API de claves.
void tratar_peticion(struct message *pr) {
        int ret, qr;
     
        switch (pr->op) {
                case 1: // DESTROY
                        pr->res = destroy();
                        break;
                case 2: // SET VALUE
                        pr->res = set_value(pr->key, pr->value1, pr->N_value2, pr->V_value2, pr->value3);
                        break;
                case 3: // GET VALUE
                        pr->res = get_value(pr->key, pr->value1, &pr->N_value2, pr->V_value2, &pr->value3);
                        break;
                case 4: // MODIFY VALUE
                        pr->res = modify_value(pr->key, pr->value1, pr->N_value2, pr->V_value2, pr->value3);
                        break;
                case 5: // DELETE KEY
                        pr->res = delete_key(pr->key);
                        break;
                case 6: // EXIST
                        pr->res = exist(pr->key);
                        break;
                default: // Operación desconocida.
                        pr->res = -1;
                        break;
        }
 
        // Abre la cola del cliente (cuya ruta se encuentra en q_name) en modo escritura.
        qr = mq_open(pr->q_name, O_WRONLY);
        if (qr < 0) {
                perror("mq_open");
                return;
        }
 
        // Envía la respuesta (la estructura message con el resultado de la operación).
        ret = mq_send(qr, (char *)pr, sizeof(struct message), 0);
        if (ret < 0)
                perror("mq_send");
 
        ret = mq_close(qr);
        if (ret < 0)
                perror("mq_close");
}
 
int main(int argc, char *argv[]) {
        int qs, ret;
        struct message pr;
        struct mq_attr attr;
        unsigned int prio;
        struct sigaction new_action, old_action;

        // Inicializa los atributos de la cola del servidor.
        attr.mq_flags   = 0;
        attr.mq_maxmsg  = 10;
        attr.mq_msgsize = sizeof(struct message);
        attr.mq_curmsgs = 0;

        // Abre (o crea si no existe) la cola del servidor en modo lectura.
        qs = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0664, &attr);
        if (qs < 0) {
                perror("mq_open");
                return -1;
        }

        // Configura el manejador de SIGINT para finalizar el servidor de forma segura.
        new_action.sa_handler = sigHandler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;
        sigaction(SIGINT, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN) {
                sigaction(SIGINT, &new_action, NULL);
        }

        // Bucle principal: recibe y procesa las peticiones de los clientes.
        while (!do_exit) {
                ret = mq_receive(qs, (char *)&pr, sizeof(struct message), &prio);
                if (ret < 0) {
                        perror("mq_receive");
                        continue;
                }
                tratar_peticion(&pr);
        }

        // Cierra y elimina la cola del servidor antes de terminar.
        ret = mq_close(qs);
        if (ret < 0)
                perror("mq_close");

        ret = mq_unlink(SERVER_QUEUE);
        if (ret < 0)
                perror("mq_unlink");

        printf("The End.\n");
        return 0;
}
 