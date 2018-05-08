#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <sys/types.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h>
#include <unistd.h> // for close
#include "read_line.h"
#include <stdlib.h>

#define MAX_LINE	256


int main(int argc , char *argv[]){ /*argv[1] is the server*/
  int sock;
  struct sockaddr_in server;
	struct hostent *hp;
  char buffer[MAX_LINE];
  int res;
  int n, confirmation, length;

	if(argc != 2){ /*checks that there is a server name as input*/
		printf("Argumet missing");
    return -1;
	}

  sock = socket(AF_INET , SOCK_STREAM , 0); /*Create socket*/
  if (sock == -1){ /*check for errors*/
    printf("Could not create socket");
  }
  printf("Socket created\n");

	bzero((char *) &server, sizeof(server)); /*reventamos mucho*/
	hp = gethostbyname(argv[1]); /*I get the host*/

	memcpy(&(server.sin_addr), hp -> h_addr, hp -> h_length);

  server.sin_family = AF_INET;
  server.sin_port = htons( 4200 );

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
    printf("connect failed. Error\n");
    return 1;
  }

  printf("Connected\n");

  /*keep communicating with server*/

    n = readLine(0, buffer, MAX_LINE); /*read from the standard input*/
    if(n < 0){ /*check errors*/
      printf("Error reading\n");
      return -1;
    }
    // length = htons(n); /*store the length of the message*/
    // printf("Length of the message sent to the server\n");
    // if( send(sock , (char *) &length, sizeof(int), 0) < 0){ /*send the length if the string*/
    //   printf("Send failed\n");
    //   return 1;
    // }
    //
    // printf("Confirmation arrived\n");
    // if( recv(sock , &confirmation , sizeof(int) , 0) < 0){ /*Receive the received confirmation from the server*/
    //   printf("recv failed\n");
    //   return 1;
    // }
    // if(confirmation < 0){ /*check that the confirmation is the expected one*/
    //   printf("confirmation failed\n");
    //   return 1;
    // }

    // if( send(sock , (char *) buffer, sizeof(char) * n, 0) < 0){ /*send the string read*/
    //   printf("Send failed\n");
    //   return 1;
    // }
    // res = (char *) malloc(sizeof(char) + sizeof(char) * n); /*allocate the memory to the response string*/
    // res[n] = '\0';
    if( send_msg(sock , (char *) buffer, MAX_LINE )< 0){ /*send the string read*/
        printf("Send failed\n");
        return 1;
      }
      bzero(&res, sizeof(res));
    if( recv(sock , &res , sizeof(int), 0) < 0){ /*Receive a reply from the server*/
      printf("recv failed\n");
      return 1;
    }
    printf("Server reply : %d\n", res);

  if(close(sock) < 0){ /*close the client socket*/
    perror("error closing the socket");
    return -1;
  }
  return 0;
}
