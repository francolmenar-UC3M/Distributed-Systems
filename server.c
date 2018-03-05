#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <stdlib.h>
#define MAX_LINE	256


int main(int argc , char *argv[]){
  int socket_desc , client_sock , c, n, length, confirmation;
	char *res;
  struct sockaddr_in server , client;

  socket_desc = socket(AF_INET , SOCK_STREAM , 0); /*Create socket*/
  if (socket_desc == -1){ /*check for errors*/
    printf("Could not create socket");
    return -1;
  }

	int val = 1;
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

  server.sin_family = AF_INET; /*Prepare the sockaddr_in structure*/
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( 4200 );

  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){ /*Bind*/
    perror("bind failed. Error");
    return 1;
  }
  printf("bind done\n");

  listen(socket_desc , 3); /*Listen*/
  c = sizeof(struct sockaddr_in); /*Accept and incoming connection*/

  while(1){
    printf("Waiting for incoming connections...\n");
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c); /*accept connection from a client*/
    if (client_sock < 0){ /*check for errors*/
        perror("accept failed");
        return 1;
    }

    n = recv(client_sock, (char *) &length, sizeof(int), 0); /*recieve the length of the string*/
    if(n < 0){ /*check for errors receiving*/
      printf("Error receiving\n");
      return -1;
    }
    length = ntohs(length);/*convert the length received from the network*/
    printf("Length of %i received\n",length);

    if(length > 0) confirmation = 0;/*check the validity of the length received*/
    else confirmation = -1;

    if( send(client_sock , (char *) &confirmation, sizeof(int), 0) < 0){ /*send the confirmation to the client*/
      printf("Send failed\n");
      return 1;
    }
    res = (char *) malloc(sizeof(char) + sizeof(char) * n); /*allocate the memory to the input string*/
    res[n] = '\0';
    n = recv(client_sock, (char *) res, sizeof(char) * n, 0); /*recieve the string*/
    if(n < 0){ /*check for errors receiving*/
      printf("Error receiving\n");
      return -1;
    }
    printf("Message received: %s\n", res);
    for(int i = 0; i < length; i++){ /*change the input message*/
      res[i] = res[i] + 1;
    }
    printf("Message sent %s\n",res );
    if(send( client_sock, (char *) res, sizeof(char) * n, 0) < 0){ /*send result*/
      perror("error sending the request");
      return -1;
    }
    printf("sent\n");

    close(client_sock); /*close client socket*/
  }
  close(socket_desc); /*close server socket*/
  return 0;
}
