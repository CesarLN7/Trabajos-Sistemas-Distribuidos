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
libclaves: proxy-rpc.c claves-rpc_clnt.c claves-rpc_xdr.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -fPIC -shared $^ -o ./lib/$@.so $(LDLIBS)

# Compilación programas principales
servidor: servidor-rpc.c claves-rpc_svc.c claves-rpc_xdr.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out $(LDLIBS)  

cliente1: app-cliente-1.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

cliente2: app-cliente-2.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

cliente3: app-cliente-3.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

cliente4: app-cliente-4.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

# Limpieza archivos generados
clean:
	rm -f *.o *.so ./lib/*.so *.out -d ./lib ./mensajes/* ./mensajes

.SUFFIXES:
.PHONY : clean