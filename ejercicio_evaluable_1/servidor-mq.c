/* aquí implementación del Servidor (primero sencillo) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>

#include "claves/claves.h"
#include "mensaje.h"


void tratar_peticion ( struct message * pr )
{
    int ret ;
    int qr ;

    switch (pr->op)
    {
        case 1: // DESTROY
                pr->res = destroy();//add(&(pr->value), pr->a, pr->b)
		printf(" %d = destroy(void);\n",    pr->res) ;
                break ;
        /*case 2: // SET VALUE
                pr->res = set_value(pr->key, char *value1, pr->N_value2, double *V_value2, struct Coord value3);//divide(&(pr->value), pr->a, pr->b) ;
		printf(" %d = set_value(%d, %d, %d);\n",    pr->res, pr->value, pr->a, pr->b) ;
                break ;
        case 3: // GET VALUE
                pr->res = get_value(pr->key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);//neg(&(pr->value), pr->a) ;
		printf(" %d = neg(%d, %d);\n",    pr->res, pr->value, pr->a) ;
                break ;
        case 4: // MODIFY VALUE
                pr->res = modify_value(pr->key, char *value1, pr->N_value2, double *V_value2, struct Coord value3);//add(&(pr->value), pr->a, pr->b) ;
		printf(" %d = add(%d, %d, %d);\n",    pr->res, pr->value, pr->a, pr->b) ;
                break ;
        case 5: // DELETE KEY
                pr->res = delete_key(pr->key);//divide(&(pr->value), pr->a, pr->b) ;
		printf(" %d = divide(%d, %d, %d);\n",    pr->res, pr->value, pr->a, pr->b) ;
                break ;
        case 6: // EXIST
                pr->res = exist(pr->key);//neg(&(pr->value), pr->a) ;
		printf(" %d = neg(%d, %d);\n",    pr->res, pr->value, pr->a) ;
                break ;*/
	default:
		printf(" unknown op:%d\n", pr->op) ;
                break ;
    }

    qr = mq_open(pr->q_name, O_WRONLY) ;
    if (qr < 0) {
	perror("mq_open: ") ;
        mq_close(qr);
	return ;
    }

    ret = mq_send(qr, (char *)pr, sizeof(struct message), 0) ;
    if (ret < 0) {
	perror("mq_send: ") ;
    }

    ret = mq_close(qr);
    if (ret < 0) {
	perror("mq_close: ") ;
    }
}


char *q_name  = "/SERVIDOR" ;
int   do_exit = 0 ;

void sigHandler ( int signo )
{
    do_exit = 1 ;
}

int main ( int argc, char *argv[] )
{
    int ret ;
    struct message pr ;
    int qs ;
    struct mq_attr attr ;
    unsigned int prio ;
    struct sigaction new_action, old_action;

    // Initialize the queue attributes (attr)
    attr.mq_flags   = 0 ;
    attr.mq_maxmsg  = 10 ;
    attr.mq_msgsize = sizeof(struct message) ;
    attr.mq_curmsgs = 0 ;

    // open server queue
    qs = mq_open(q_name, O_CREAT|O_RDONLY, 0664, &attr) ;
    if (qs < 0) {
	perror("mq_open: ") ;
	return -1 ;
    }

    // signal handler
    new_action.sa_handler = sigHandler ;
    sigemptyset (&new_action.sa_mask) ;
    new_action.sa_flags = 0 ;
    sigaction (SIGINT, NULL, &old_action) ;
    if (old_action.sa_handler != SIG_IGN) {
        sigaction (SIGINT, &new_action, NULL);
    }

    // receive and treat requests
    while (0 == do_exit)
    {
        ret = mq_receive(qs, (char *)&pr, sizeof(struct message), &prio) ;
        if (ret < 0) {
	    perror("mq_receive: ") ;
	    continue;
        }

        tratar_peticion(&pr) ;
    }

    // end
    ret = mq_close(qs) ;
    if (ret < 0) {
	perror("mq_close: ") ;
    }

    ret = mq_unlink(q_name);
    if (ret < 0) {
	perror("mq_unlink: ") ;
    }

    printf("The End.\n") ;
    return 0 ;
}