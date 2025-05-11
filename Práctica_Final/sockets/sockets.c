#include <unistd.h>
#include <errno.h>
#include "sockets.h"

int send_message(int socket, char *buffer, int len) {
	int r;
	int l = len;
	do {	
		r = write(socket, buffer, l);
		l = l - r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo al escribir */
	else
		return(0);	/* se han enviado al socket len bytes del buffer */
}

int receive_message(int socket, char *buffer, int len) {
	int r;
	int l = len;
	do {	
		r = read(socket, buffer, l);
		l = l - r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo al leer */
	else
		return(0);	/* se han recibido del socket len bytes al buffer */
}

ssize_t readLine(int fd, void *buffer, size_t n)
{
	ssize_t number_read;  /* número de bytes recuperados por el último read() */
	size_t total_read;	  /* bytes leídos en total */
	char *buf;
	char ch;


	if (n <= 0 || buffer == NULL) { 
		errno = EINVAL;
		return -1; 
	}

	buf = buffer;
	total_read = 0;
	
	for (;;) {
        	number_read = read(fd, &ch, 1);	/* se lee un byte */

        	if (number_read == -1) {	
            		if (errno == EINTR)	/* interrumpido -> volver a hacer read() */
                		continue;
            	else
			return -1;		/* cualquier otro error */
        	} else if (number_read == 0) {	/* EOF */
            		if (total_read == 0)	/* no se lee ningún byte; return 0 */
                		return 0;
			else
                		break;
        	} else {			/* number_read debe ser 1 si llegamos hasta aquí*/
            		if (ch == '\n')
                		break;
            		if (ch == '\0')
                		break;
            		if (total_read < n - 1) {		/* se descartan > (n-1) bytes */
				total_read++;
				*buf++ = ch; 
			}
		} 
	}
	
	*buf = '\0';
    	return total_read;
}
