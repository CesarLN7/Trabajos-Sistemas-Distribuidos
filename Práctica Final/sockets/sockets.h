#ifndef SOCKETS_H
#define SOCKETS_H

int send_message(int socket, char * buffer, int len);

int receive_message(int socket, char *buffer, int len);

ssize_t readLine(int fd, void *buffer, size_t n);
 
#endif