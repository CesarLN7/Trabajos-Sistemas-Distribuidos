CC = gcc
CLAVES_PATH = claves
CFLAGS = -lrt
OBJS = servidor cliente 
BIN_FILES = servidor cliente

all: $(OBJS)

# Compilar la librería dinámica a partir de proxy-mq.c
libclaves.so: proxy-mq.c
	$(CC) -fPIC -c -o proxy-mq.o proxy-mq.c
	$(CC) -shared -fPIC -o libclaves.so proxy-mq.o

# Compilar el servidor: utiliza servidor-mq.c y claves/claves.c
servidor: servidor-mq.c $(CLAVES_PATH)/claves.c
	$(CC) $(CFLAGS) $^ -o servidor.out

# Compilar el cliente: se enlaza con libclaves.so para que use el proxy
cliente: app-cliente.c libclaves.so
	$(CC) $(CFLAGS) -o cliente.out app-cliente.c -L. -lclaves -lrt -Wl,-rpath,.

	
clean:
	rm -f $(BIN_FILES) *.out *.o *.so $(CLAVES_PATH)/*.o data.txt

re:	clean all

.PHONY: all libclaves.so servidor cliente clean re