CC = gcc
CFLAGS = -g

LIBS = -lpthread

PRGS = client server testServerList

all: libkeys.a $(PRGS)

keys: keys.c
		$(CC) $(CFLAGS) -Wall -o keys keys.c $(LIBS) -lm -lrt

libkeys.a: keys.o
	ar rcs libkeys.a keys.o

client: client.c
	$(CC) $(CFLAGS) -Wall -o client client.c  libkeys.a $(LIBS) -L -lm -lrt

server: server.c
	$(CC) $(CFLAGS) -o server server.c doubleLinkedList.c $(LIBS) -lm -lrt

testServerList: testServerList.c
	$(CC) $(CFLAGS) -o testServerList testServerList.c testServerListAux.c doubleLinkedList.c $(LIBS) -lm -lrt

clean:
	rm -fr *o *a $(PRGS)
