#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<sys/types.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <netdb.h>
#include <unistd.h> // for close
 
int main(int argc , char *argv[])
{ /*argv[1] is the server*/
    int sock;
    struct sockaddr_in server;
	struct hostent *hp;
    char server_reply[2000];
	int num [2], res;

	if(argc != 2){
		printf("Argumet missing");
	}
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created\n");

	bzero((char *) &server, sizeof(server)); /*reventamos mucho*/
	hp = gethostbyname(argv[1]);

	memcpy(&(server.sin_addr), hp -> h_addr, hp -> h_length);
     
    server.sin_family = AF_INET;
    server.sin_port = htons( 4200 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("connect failed. Error\n");
        return 1;
    }
     
    printf("Connected\n");

	num[0] = htons(2);
	num[1] = htons(4);
     
    //keep communicating with server
         
    //Send some data
    if( send(sock , (char *) num, 2*sizeof(int), 0) < 0)
    {
        printf("Send failed\n");
        return 1;
     }
     printf("Sent\n");   
     //Receive a reply from the server
     if( recv(sock , &res , sizeof(int) , 0) < 0)
     {
         printf("recv failed\n");
        return 1;
     }
         
    printf("Server reply : %d\n", ntohs(res));
     
    close(sock);
    return 0;
}
