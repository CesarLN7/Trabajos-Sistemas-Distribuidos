CC = gcc
CLAVES_PATH = claves
CFLAGS = -lrt
OBJS = servidor cliente 
BIN_FILES = servidor cliente

all: $(OBJS)

libclaves.so: $(CLAVES_PATH)/claves.c
	$(CC) -fPIC -c -o $(CLAVES_PATH)/claves.o $<
	$(CC) -shared -fPIC -o $@ $(CLAVES_PATH)/claves.o

servidor:  servidor.c operaciones/operaciones.c
	$(CC)  $(CFLAGS) $^ -o $@.out

cliente: cliente.c libclaves.so
	$(CC) -L. -lclaves $(CFLAGS) -o $@.out $< ./libclaves.so -lrt

clean:
	rm -f $(BIN_FILES) *.out *.o *.so $(CLAVES_PATH)/*.o data.txt

re:	clean all

.PHONY: all libclaves.so servidor cliente clean re