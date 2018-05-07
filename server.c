#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<sys/types.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <netdb.h>
#include<unistd.h>    //write
#include "read_line.h"
#include <stdlib.h>
#define MAX_LINE	256

/* Read an int from the given socket
 *
 * @param sock: the socket we are using to transfer data
 * @return the int read
 */
int receiveInt(int sock) {
  char intBufferCoupReq[MAX_LINE]; /* buffer */
  memset(intBufferCoupReq, '\0', sizeof(intBufferCoupReq)); /* end of string char */

  int k = 0; /* no se por qué se pone esto */
  while ( 1 ) { /* reading from the socket */
    int nbytes = recv(sock, &intBufferCoupReq[k], 1, 0);
    if ( nbytes == -1 ) { printf("recv error\n"); return -1; }
    if ( nbytes ==  0 ) { break; }
    k++;
  }
  return ntohl(*((int *) &intBufferCoupReq));
}

/* Read a string from the given socket
 *
 * @param sock: the socket we are using to transfer data
 * @return -1 if error
 */
int receiveString(int sock, char* input) {
  memset(input, '\0', sizeof(*input)); /* end of string char */


  int k = 0; /* no se por qué se pone esto */
  while ( 1 ) { /* reading from the socket */
    int nbytes = recv(sock, &input[k], 1, 0);
    if ( nbytes == -1 ) { printf("recv error\n"); return -1; }
    if ( nbytes ==  0 ) { break; }
    if ( input[k] ==  '\0' ) { break; } /* end of string */
      k++;
  }
  return 0;
}

int sendByte(int sock, char *input) {
  if(send_msg( sock, (char *) input, sizeof(char)) < 0){ /*send result*/
    perror("error sending the request");
    return -1;
  }
  return 0;
}

int sendString(int sock, char *input) {
    if(send_msg( sock, (char *) input, sizeof(char) * MAX_LINE) < 0){ /*send result*/
        perror("error sending the request");
        return -1;
    }
    return 0;
}

int connectOp(int client_sock){
  char port[MAX_LINE];

  if( receiveString(client_sock, port) < 0){
    perror("receive string failed");
    return 1;
  }
  int portN = atoi(port);
  printf("Port: %d\n", portN);


  char * msg = "SEND MESSAGE\0";
  printf("Message to send: %s\n", msg);

  /*** Create socket ***/
  int sock;
  struct sockaddr_in server;
  struct hostent *hp;

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
    printf("Could not create socket");
  }
  printf("Socket created\n");
  char * ip = "163.117.218.98\0";

  bzero((char *) &server, sizeof(server)); /*reventamos mucho*/

  hp = gethostbyaddr( (char *) &ip, sizeof(ip), AF_INET);

  if(hp == NULL) {
    printf("Error hp \n");
  }

  memcpy(&(server.sin_addr), hp -> h_addr, hp -> h_length);

  server.sin_family = AF_INET;
  server.sin_port = htons( portN );

  printf("%d\n", portN);
  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    printf("connect failed. Error\n");
    return 1;
  }

  printf("Connected\n");

  sendString(sock, msg);
  printf("Sent\n");

  return 0;
}

int main(int argc , char *argv[]){
  int socket_desc , client_sock , c;
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
  server.sin_port = htons( 1025 );

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

      /* Receive an int */
//    int length;
//    if( (length = receiveInt(client_sock) ) < 0){
//      perror("receive int failed");
//      return 1;
//    }
//    printf("length: %i\n", length);
//    close(client_sock); /*close client socket*/


    //client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c); /*accept connection from a client*/
    char operation[MAX_LINE];
    if( receiveString(client_sock, operation) < 0){
      perror("receive string failed");
      return 1;
    }
    printf("Operation: %s\n", operation);

    char username[MAX_LINE];

      if( receiveString(client_sock, username) < 0){
      perror("receive string failed");
      return 1;
    }
    printf("User: %s\n", username);

    if(strcmp(operation, "CONNECT") == 0){
      
     // connectOp(client_sock);
    }

    //close(client_sock); /*close client socket*/

    char byte[1];
    byte[0] = 0x00;
    //client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c); /*accept connection from a client*/
    if( sendByte(client_sock, byte) < 0){ /* send byte */
      perror("send byte failed");
      return 1;
    }
    close(client_sock); /*close client socket */


  }
  close(socket_desc); /*close server socket*/
  return 0;
}
