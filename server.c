#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
	int num [2], res;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    printf("Socket casi\n");
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created\n");
    
	int val = 1;
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 4200 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    c = sizeof(struct sockaddr_in);
    while(1){
    	printf("Waiting for incoming connections...\n");
		//accept connection from an incoming client
    	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    	if (client_sock < 0)
    	{
       	 	perror("accept failed");
       		 return 1;
    	}
		recv(client_sock, (char *)num, 2*sizeof(int), 0); /*recieve request*/ 
   		printf("Recivo\n");
        printf("numero1: %d , numero2: %d\n", ntohs(num[0]) , ntohs(num[1]));
		res = ntohs(num[0]) + ntohs(num[1]); /*process request*/
		printf("res: %d\n", res);
		res = htons(res);
    	send( client_sock, &res, sizeof(int), 0);/*send result*/
    	printf("sent\n");
		close(client_sock);
	}
    close(socket_desc);
    return 0;
}
