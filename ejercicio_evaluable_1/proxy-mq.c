/*implementar funciones con colas (al principio vacías). -2 cuando fallo*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>

#include "proxy-mq.h"
#include "mensaje.h"
#include "claves/claves.h"

int d_send_receive ( struct message *pr )
{
    int  ret ;
    char qr_name[1024] ;
    struct mq_attr attr;
    int qs, qr ;
    unsigned prio ;

     /* Initialize the queue attributes (attr) */
    bzero(&attr, sizeof(struct mq_attr)) ;
    attr.mq_flags   = 0 ;
    attr.mq_maxmsg  = 10 ;
    attr.mq_msgsize = sizeof(struct message) ;
    attr.mq_curmsgs = 0 ;

  // qs = mq_open("/SERVIDOR", O_WRONLY, 0700, &attr) ;
    qs = mq_open("/SERVIDOR", O_WRONLY) ;
    if (qs == -1) {
        perror("mq_open: ") ;
        return -1 ;
    }

    sprintf(qr_name, "%s%d", "/CLIENTE_", getpid()) ;
    qr = mq_open(qr_name, O_CREAT|O_RDONLY, 0664, &attr) ;
    if (qr == -1) {
	    perror("mq_open: ") ;
	    mq_close(qs) ;
	    return -1;
    }
    strcpy(pr->q_name, qr_name) ;

    // send request
    ret = mq_send(qs, (char *)pr, sizeof(struct message), 0) ;
    if (ret < 0) {
	    perror("mq_send: ") ;
	    q_close(qs) ;
	    mq_close(qr) ;
            mq_unlink(qr_name) ;
	    return -1;
    }

    // receive response
    ret = mq_receive(qr, (char *)pr, sizeof(struct message), &prio) ;
    if (ret < 0) {
	    perror("mq_receive: ") ;
	    mq_close(qs) ;
	    mq_close(qr) ;
            mq_unlink(qr_name) ;
	    return -1;
    }

    // close queues
    ret = mq_close(qs);
    if (ret < 0) {
	    perror("mq_close: ") ;
    }
    ret = mq_close(qr);
    if (ret < 0) {
	    perror("mq_close: ") ;
    }
    ret = mq_unlink(qr_name);
    if (ret < 0) {
	    perror("mq_unlink: ") ;
    }

    // return status
    return pr->status ;
}

int q_destroy() {
    struct message pr;

    // init message
    bzero(&pr, sizeof(struct message)) ;
    pr.op    = 1 ;

    // send request and receive response
    d_send_receive(&pr) ;

    // return status
    return pr.res ;
};

/*int q_set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { printf("set_value"); return 1; };

int q_get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) { printf("get_value"); return 1; };

int q_modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) { printf("modify_value"); return 1; };

int q_delete_key(int key) { printf("delete_key"); return 1; };

int q_exist(int key) { printf("exist"); return 1; };*/