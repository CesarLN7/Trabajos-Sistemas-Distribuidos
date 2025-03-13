#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
 
#include "proxy-mq.h"
#include "mensaje.h"
#include "claves/claves.h"
 
#define SERVER_QUEUE "/SERVIDOR"  // Nombre de la cola del servidor
 
// Función auxiliar que envía una petición al servidor y espera la respuesta.
// Se crea una cola exclusiva para el cliente (identificada por el PID) y, una vez
// recibida la respuesta, se cierra y elimina dicha cola.
int d_send_receive(struct message *pr) {
    int ret;
    char qr_name[MAXSTR];
    struct mq_attr attr;
    int qs, qr;
    unsigned prio;

    // Inicializa los atributos de la cola para el cliente.
    memset(&attr, 0, sizeof(struct mq_attr));
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof(struct message);
    attr.mq_curmsgs = 0;

    // Abre la cola del servidor en modo escritura.
    qs = mq_open(SERVER_QUEUE, O_WRONLY);
    if (qs == -1) {
        perror("mq_open");
        return -2;
    }
    
    // Crea el nombre de la cola exclusiva para este cliente.
    sprintf(qr_name, "%s%d", "/CLIENTE_", getpid());
    qr = mq_open(qr_name, O_CREAT | O_RDONLY, 0664, &attr);
    if (qr == -1) {
        perror("mq_open");
        mq_close(qs);
        return -2;
    }
    // Se copia el nombre de la cola del cliente en el mensaje.
    strcpy(pr->q_name, qr_name);

    // Envía la petición al servidor.
    ret = mq_send(qs, (char *)pr, sizeof(struct message), 0);
    if (ret < 0) {
        perror("mq_send");
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -2;
    }
 
    // Espera la respuesta en la cola exclusiva del cliente.
    ret = mq_receive(qr, (char *)pr, sizeof(struct message), &prio);
    if (ret < 0) {
        perror("mq_receive");
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -2;
    }
 
    // Cierra y elimina la cola del cliente.
    mq_close(qs);
    mq_close(qr);
    mq_unlink(qr_name);
 
    // Devuelve el resultado de la operación (campo res).
    return pr->res;
}
 
// Implementa q_destroy: inicializa el servicio borrando todas las tuplas.
int destroy() {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 1;  // Código de operación para destroy
    return d_send_receive(&pr);
}
 
// Implementa q_set_value: inserta la tupla <key, value1, value2, value3>.
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 2;  // Código para set_value
    pr.key = key;
    strncpy(pr.value1, value1, MAXSTR-1);
    pr.N_value2 = N_value2;  // Almacena la dimensión
    memcpy(pr.V_value2, V_value2, N_value2 * sizeof(double));
    pr.value3 = value3;  // Copia la estructura Coord
    return d_send_receive(&pr);
}
 
// Implementa q_get_value: recupera la tupla asociada a 'key'.
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 3;  // Código para get_value
    pr.key = key;
    int res = d_send_receive(&pr);
    if (res == 0) {
        strncpy(value1, pr.value1, MAXSTR-1);
        *N_value2 = pr.N_value2;
        memcpy(V_value2, pr.V_value2, (*N_value2) * sizeof(double));
        *value3 = pr.value3;
    }
    return res;
}
 
// Implementa q_modify_value: modifica la tupla asociada a 'key'.
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 4;  // Código para modify_value
    pr.key = key;
    strncpy(pr.value1, value1, MAXSTR-1);
    pr.N_value2 = N_value2;
    memcpy(pr.V_value2, V_value2, N_value2 * sizeof(double));
    pr.value3 = value3;
    return d_send_receive(&pr);
}
 
// Implementa q_delete_key: elimina la tupla asociada a 'key'.
int delete_key(int key) {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 5;  // Código para delete_key
    pr.key = key;
    return d_send_receive(&pr);
}
 
// Implementa q_exist: verifica si existe la tupla asociada a 'key'.
int exist(int key) {
    struct message pr;
    memset(&pr, 0, sizeof(struct message));
    pr.op = 6;  // Código para exist
    pr.key = key;
    return d_send_receive(&pr);
}
 