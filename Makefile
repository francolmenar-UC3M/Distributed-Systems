all: server client

server: server.c
		gcc -o server server.c doubleLinkedList.c -lpthread -lm -lrt

client: client.c
		gcc -o client client.c keys.c -lpthread -lm -lrt

clean:
		rm -fr *~ server client
