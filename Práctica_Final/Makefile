CC = gcc
CFLAGS = -lrt -fPIC -Wall -Wextra -g

LOGIC_PATH = logic
SOCKETS_PATH = sockets

SRCS = server.c $(LOGIC_PATH)/logic.c $(SOCKETS_PATH)/sockets.c
OBJS = server.o logic.o sockets.o
TARGET = server.out

.PHONY: all clean re

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

logic.o: $(LOGIC_PATH)/logic.c
	$(CC) $(CFLAGS) -c -o $@ $<

sockets.o: $(SOCKETS_PATH)/sockets.c
	$(CC) $(CFLAGS) -c -o $@ $<

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.out *.so users.txt contents_*.txt
	rm -rf tmp/copias/*
	
re: clean all