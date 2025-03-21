CC = gcc
CLAVES_PATH = claves
CFLAGS = -lrt
OBJS = servidor cliente cliente2
BIN_FILES = servidor cliente cliente2

all: $(OBJS)

# Compilar la librería dinámica a partir de proxy-sock.c
libclaves.so: proxy-sock.c
	$(CC) -fPIC -c -o proxy-sock.o proxy-sock.c
	$(CC) -shared -fPIC -o libclaves.so proxy-sock.o

# Compilar el servidor: utiliza servidor-sock.c y claves/claves.c
servidor: servidor-sock.c $(CLAVES_PATH)/claves.c
	$(CC) $(CFLAGS) $^ -o servidor.out

# Compilar el cliente 1: se enlaza con libclaves.so para que use el proxy
cliente: app-cliente.c libclaves.so
	$(CC) $(CFLAGS) -o cliente.out app-cliente.c -L. -lclaves -lrt -Wl,-rpath,.

# Compilar el cliente 2: se enlaza con libclaves.so para que use el proxy
cliente2: app-cliente2.c libclaves.so
	$(CC) $(CFLAGS) -o cliente2.out app-cliente2.c -L. -lclaves -lrt -Wl,-rpath,.

clean:
	rm -f $(BIN_FILES) *.out *.o *.so $(CLAVES_PATH)/*.o data.txt

re: clean all

.PHONY: all libclaves.so servidor cliente cliente2 clean re