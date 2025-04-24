CC = gcc
CPPFLAGS = -Wall -Wextra -Werror
LDLIBS = -lrt -lpthread -lnsl -ltirpc
LDFLAGS = -L$(INSTALL_PATH)/lib/ -lnsl -lpthread -ldl -ltirpc 
CPPFLAGS = -D_REENTRANT
CFLAGS = -g -I/usr/include/tirpc

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

# Generación programas principales -> cliente y servidor
all: dir libclaves servidor cliente 
.PHONY : all

# Creación directorio lib
dir:
	$(shell mkdir -p lib)

# Creación librería compartida
libclaves: claves.c tuplas_clnt.c tuplas_xdr.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -fPIC -shared $^ -o ./lib/$@.so $(LDLIBS)

# Compilación programas principales
servidor: tuplas_server.c tuplas_svc.c tuplas_xdr.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out $(LDLIBS)  

cliente: cliente.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

# Limpieza archivos generados
clean:
	rm -f *.o *.so ./lib/*.so *.out -d ./lib ./mensajes/* ./mensajes

.SUFFIXES:
.PHONY : clean