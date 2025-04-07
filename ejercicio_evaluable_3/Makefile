CC = gcc
CLAVES_PATH = claves
SOCKETS_PATH = sockets
CFLAGS = -lrt
OBJS = servidor cliente1 cliente2 cliente3 cliente4
BIN_FILES = servidor cliente1 cliente2 cliente3 cliente4

all: $(OBJS)

# Compilar la librería dinámica a partir de proxy-sock.c
libclaves.so: proxy-sock.c
	$(CC) -fPIC -c -o proxy-sock.o proxy-sock.c
	$(CC) -shared -fPIC -o libclaves.so $(SOCKETS_PATH)/sockets.c proxy-sock.o

# Compilar el servidor: utiliza servidor-sock.c y claves/claves.c
servidor: servidor-sock.c $(CLAVES_PATH)/claves.c $(SOCKETS_PATH)/sockets.c
	$(CC) $(CFLAGS) $^ -o servidor.out

# Compilar el cliente 1: se enlaza con libclaves.so para que use el proxy
cliente1: app-cliente-1.c libclaves.so
	$(CC) $(CFLAGS) -o cliente1.out app-cliente-1.c -L. -lclaves -lrt -Wl,-rpath,.

# Compilar el cliente 2: se enlaza con libclaves.so para que use el proxy
cliente2: app-cliente-2.c libclaves.so
	$(CC) $(CFLAGS) -o cliente2.out app-cliente-2.c -L. -lclaves -lrt -Wl,-rpath,.

cliente3: app-cliente-3.c libclaves.so
	$(CC) $(CFLAGS) -o cliente3.out app-cliente-3.c -L. -lclaves -lrt -Wl,-rpath,.

# Compilar el cliente 2: se enlaza con libclaves.so para que use el proxy
cliente4: app-cliente-4.c libclaves.so
	$(CC) $(CFLAGS) -o cliente4.out app-cliente-4.c -L. -lclaves -lrt -Wl,-rpath,.

clean:
	rm -f $(BIN_FILES) *.out *.o *.so $(CLAVES_PATH)/*.o data.txt

re: clean all

.PHONY: all libclaves.so servidor cliente1 cliente2 cliente3 cliente4 clean re