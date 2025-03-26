#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
 
#include "claves/claves.h"
#include "mensaje.h"
 
#define SERVER_QUEUE "/SERVIDOR"   // Nombre de la cola del servidor

/* mutex y variables condicionales para proteger la copia del mensaje*/
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = true;
pthread_cond_t cond_mensaje;
 
// Función para tratar cada petición recibida.
// Dependiendo del campo 'op', se invoca la función correspondiente de la API de claves.
void tratar_peticion(void *in) {
        struct message pr; /* mensaje local */
        mqd_t qr; /* cola del cliente */
        int res; /* resultado de la operación */

        /* el thread copia el mensaje a un mensaje local */
        pthread_mutex_lock(&mutex_mensaje);
        pr = (*(struct message *) in);
        /* ya se puede despertar al servidor*/
        mensaje_no_copiado = false;
        pthread_cond_signal(&cond_mensaje);
        pthread_mutex_unlock(&mutex_mensaje);

        switch (pr.op) {
                case 1: // DESTROY
                        pr.res = destroy();
                        break;
                case 2: // SET VALUE
                        pr.res = set_value(pr.key, pr.value1, pr.N_value2, pr.V_value2, pr.value3);
                        break;
                case 3: // GET VALUE
                        pr.res = get_value(pr.key, pr.value1, &pr.N_value2, pr.V_value2, &pr.value3);
                        break;
                case 4: // MODIFY VALUE
                        pr.res = modify_value(pr.key, pr.value1, pr.N_value2, pr.V_value2, pr.value3);
                        break;
                case 5: // DELETE KEY
                        pr.res = delete_key(pr.key);
                        break;
                case 6: // EXIST
                        pr.res = exist(pr.key);
                        break;
                default: // Operación desconocida.
                        pr.res = -1;
                        break;
        }
 
        // Abre la cola del cliente (cuya ruta se encuentra en q_name) en modo escritura.
        qr = mq_open(pr.q_name, O_WRONLY);
        if (qr < 0) {
                perror("mq_open");
                return;
        }

        // Envía la respuesta (la estructura message con el resultado de la operación).
        if (mq_send(qr, (char *)&pr, sizeof(struct message), 0) < 0) {
                perror("mq_send");
                return;
        }
        
        mq_close(qr);

        pthread_exit(0);
}
 
int main(void) {
        mqd_t qs; /* cola del servidor */
        struct message pr; /* mensaje a recibir */
        struct mq_attr attr; /* atributos de la cola */
        pthread_attr_t t_attr; /* atributos de los threads */
        pthread_t thid;

        // Inicializa los atributos de la cola del servidor.
        attr.mq_flags   = 0;
        attr.mq_maxmsg  = 10;
        attr.mq_msgsize = sizeof(struct message);
        attr.mq_curmsgs = 0;

        // Abre (o crea si no existe) la cola del servidor en modo lectura.
        qs = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0664, &attr);
        if (qs < 0) {
                perror("mq_open");
                return -2;
        }

        pthread_mutex_init(&mutex_mensaje, NULL);
        pthread_cond_init(&cond_mensaje, NULL);
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

        // Bucle principal: recibe y procesa las peticiones de los clientes.
        while (true) {
                if (mq_receive(qs, (char *)&pr, sizeof(struct message), 0) < 0) {
                        perror("mq_receive");
                        return -2;
                }
                if (pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)&pr) == 0) {
                        pthread_mutex_lock(&mutex_mensaje);
                        while (mensaje_no_copiado) {
                                pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
                        }
                        mensaje_no_copiado = true;
                        pthread_mutex_unlock(&mutex_mensaje);
                }
        }

        // Cierra y elimina la cola del servidor antes de terminar.
        mq_close(qs);
      
        mq_unlink(SERVER_QUEUE);

        printf("The End.\n");
        return 0;
}
 