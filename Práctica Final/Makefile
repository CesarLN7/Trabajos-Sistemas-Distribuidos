CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -pthread
LIB_NAME = libclaves.so

SRC_LOGIC = logic/logic.c
SRC_SOCKETS = sockets/sockets.c
SRC_SERVER = server.c

OBJ_LOGIC = logic.o
OBJ_SOCKETS = sockets.o

.PHONY: all clean

all: $(LIB_NAME) server

$(LIB_NAME): $(SRC_LOGIC) $(SRC_SOCKETS)
	$(CC) $(CFLAGS) -c $(SRC_LOGIC) -o logic.o
	$(CC) $(CFLAGS) -c $(SRC_SOCKETS) -o sockets.o
	$(CC) -shared -o $(LIB_NAME) logic.o sockets.o

server: $(SRC_SERVER) $(LIB_NAME)
	$(CC) $(SRC_SERVER) -L. -lclaves -Wl,-rpath=. $(LDFLAGS) -o server.out

clean:
	rm -f *.o *.so *.out
