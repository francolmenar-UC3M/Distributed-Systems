all: libkeys.a server client testServerList

libkeys.a: keys.o
	ar rcs libkeys.a keys.o

keys: keys.c
	gcc -o keys keys.c -lpthread -lm -lrt

server: server.c
		gcc -o server server.c doubleLinkedList.c -lpthread -lm -lrt

client: client.c
		gcc -o client client.c  libkeys.a -lpthread -L -lm -lrt

testServerList: testServerList.c
	gcc -o testServerList testServerList.c testServerListAux.c doubleLinkedList.c -lpthread -lm -lrt

clean:
		rm -fr *o *a client server testServerList
