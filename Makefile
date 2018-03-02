all: libkeys.a client server testServerList testServerList

keys: keys.c
		gcc -Wall -o keys keys.c -lpthread -lm -lrt

libkeys.a: keys.o
	ar rcs libkeys.a keys.o

client: client.c
	gcc -Wall -o client client.c  libkeys.a -lpthread -L -lm -lrt

server: server.c
	gcc -o server server.c doubleLinkedList.c -lpthread -lm -lrt

testServerList: testServerList.c
	gcc -o testServerList testServerList.c testServerListAux.c doubleLinkedList.c -lpthread -lm -lrt

clean:
	rm -fr *o *a client server testServerList
