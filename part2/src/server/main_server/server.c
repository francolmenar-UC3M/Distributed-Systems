#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "user_storage.h"
#include "dlinkedlist.c"
#include "read_line.h"
#include "queue.h"

#define LISTEN_BACKLOG 50
#define MAX_LINE 256

pthread_mutex_t mutex_msg;
pthread_mutex_t message_id_lock;
pthread_cond_t cond_msg;
int sock_not_free = 1;

void* process_request(void* s);
int connect_user(int s_local, char* username);
int disconnect(char* username);
int unregister(char* username);
int send_message(char* sender, char* receiver, char* message);
int sendAttach(char* sender, char* receiver, char* message, char* fileName, char* fileContent);
int setMessage(char * str, char * dest);
int sendToClient(int socket, char * msg);
int receiveInt(int socket);
int receiveString(int sock, char* input);

int sendToClient(int socket, char * msg){
    char  str [MAX_LINE];
    setMessage(msg, str);
    send(socket, str, MAX_LINE, MSG_NOSIGNAL);
    return 0;
}

int setMessage(char *str, char * dest){
    char msg[MAX_LINE];
    memset(msg, '\0', sizeof(msg));
    strcpy(msg, str);
    strcpy(dest, msg);
    return 0;
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

int main(int argc, char* argv[]) {
    char* server_ip;
    int server_port;
    int server_socket, client_socket;
    struct sockaddr_in server, client;
    struct ifreq ifr;
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);

    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
      fprintf(stderr, "%s\n\n", "usage: ./server -p <port>");
      return -1;
    }

    next_message_id = 0;

    CLIENT* clnt;
    clnt = clnt_create ("localhost", USERSTORAGE, USERSTORAGEVER, "tcp");
    if (clnt == NULL) {
      fprintf(stderr, "s> INIT SERVER FAIL\n");
      return 2;
  	}
    init_1(clnt);
    clnt_destroy(clnt);

    server_port = atoi(argv[2]);
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // Open socket

    int val;
    val = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

    if (server_socket == -1) {
      fprintf(stderr, "%s\n", "ERROR socket cannot be opened");
      return -1;
    }

    bzero((char *)&ifr, sizeof(struct ifreq));
    ifr.ifr_addr.sa_family = AF_INET; // Set family IPv4
    //snprintf(ifr.ifr_name, IFNAMSIZ, "eth0");
    snprintf(ifr.ifr_name, IFNAMSIZ, "lo");
    ioctl(server_socket, SIOCGIFADDR, &ifr);
    server_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    /**********************************************/

    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    if (inet_aton(server_ip, &server.sin_addr) == 0) {
      fprintf(stderr, "%s\n", "ERROR invalid IP address");
      return -1;
    }
    server.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1) {
      fprintf(stderr, "%s\n", "ERROR binding failed");
      return -1;
    }

    if (listen(server_socket, LISTEN_BACKLOG) == -1) {
      fprintf(stderr, "%s\n", "ERROR listening failed");
      return -1;
    }

    pthread_attr_t t_attr;
    pthread_mutex_init(&mutex_msg, NULL);
    pthread_mutex_init(&message_id_lock, NULL);
    pthread_cond_init(&cond_msg, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    pthread_t thr;

    printf("s> init server %s:%d\n", server_ip, server_port);
    printf("s> \n");

    while(TRUE) {

      bzero((char *)&client, sizeof(struct sockaddr_in));
      client_socket = accept(server_socket, (struct sockaddr *)&client, &peer_addr_size);
      pthread_create(&thr, &t_attr, &process_request, (void *)&client_socket);

      pthread_mutex_lock(&mutex_msg);
      while(sock_not_free) {
        pthread_cond_wait(&cond_msg, &mutex_msg);
      }
      sock_not_free = TRUE;
      pthread_mutex_unlock(&mutex_msg);

    }
}

int user_register(struct sockaddr_in* client_addr, char *username) {
  if (search(username) != NULL) {
    fprintf(stderr, "s> REGISTER %s FAIL\n", username);
    return 1;
  } else {
    struct user *new_user = (struct user*)malloc(sizeof(struct user));
    strcpy(new_user->username, username);
    new_user->status = 0;
    new_user->ip_address = 0;
    new_user->port = ntohs(client_addr->sin_port);
    new_user->pending_messages = ConstructQueue(10);
    new_user->last_message = 0;

    Node* new_node = getNewNode(new_user);

    CLIENT* clnt;
    clnt = clnt_create ("localhost", USERSTORAGE, USERSTORAGEVER, "tcp");
    if (clnt == NULL) {
      fprintf(stderr, "s> REGISTER %s FAIL\n", username);
      return 2;
  	}
    register_user_1(username, clnt);
    clnt_destroy(clnt);

    insert(new_node);
    printf("s> REGISTER %s OK\n", username);
  }
  return 0;
}

int process_data(int s_local, char* operation) {
  /* SENDER */
  char argument1[MAX_LINE];

  if (readLine(s_local, argument1, MAX_LINE) == -1) {
    fprintf(stderr, "s> ERROR reading line\n");
    return 2;
  }

  if (operation == NULL || argument1 == NULL) {
    printf("s> ERROR MESSAGE FORMAT\n");
    return 2;
  }

  if (strcmp(operation, "REGISTER\0") == 0) {
    struct sockaddr_in client_addr;
    socklen_t size;
    size = sizeof(struct sockaddr_in);
    getpeername(s_local, (struct sockaddr *)&client_addr, &size);
    int result;
    result = user_register(&client_addr, argument1);
    return result;

  } else if (strcmp(operation, "UNREGISTER\0") == 0) {
    return unregister(argument1);

  } else if (strcmp(operation, "CONNECT\0") == 0) {
    return connect_user(s_local, argument1);

  } else if (strcmp(operation, "DISCONNECT\0") == 0) {
    return disconnect(argument1);
  }
    /* RECEIVER */
    char argument2[MAX_LINE];
    if (readLine(s_local, argument2, MAX_LINE) == -1) {
      fprintf(stderr, "s> ERROR reading line\n");
      return 2;
    }
    /* MESSAGE */
    char argument3[MAX_LINE];
    if (readLine(s_local, argument3, MAX_LINE) == -1) {
      fprintf(stderr, "s> ERROR reading line\n");
      return 2;
    }
    else if (strcmp(operation, "SEND\0") == 0) {
      return send_message(argument1, argument2, argument3);
    } else if (strcmp(operation, "SENDATTACH\0") == 0) {
      /* FILE NAME */
      char argument4[MAX_LINE];
      if (readLine(s_local, argument4, MAX_LINE) == -1) {
        fprintf(stderr, "s> ERROR reading line\n");
        return 2;
      }

        /* FILE SIZE */
        char argument5[4];
        recv(s_local, argument5, 4, 0);

        int size = ntohl(*((int *) &argument5));


        /* FILE CONTENT */
      char argument6[size+1];
      printf("SIZE: %d\n", size);
      if (recv(s_local, argument6, (size+1), 0) < 0) {
        fprintf(stderr, "s> ERROR reading line\n");
        return 2;
      }
      // printf("FILE CONTENT: %s\n", argument6);


      return sendAttach(argument1, argument2, argument3, argument4, argument6);
    } else {
      fprintf(stderr, "s> ERROR MESSAGE FORMAT");
      return 2;
    }
  return 0;
}

void* process_request(void* s) {
  int s_local;
  char return_code;
  int error;

  pthread_mutex_lock(&mutex_msg);
  s_local = *(int*)s;
  sock_not_free = FALSE;
  pthread_cond_signal(&cond_msg);
  pthread_mutex_unlock(&mutex_msg);

  char operation[MAX_LINE];
  bzero(operation, MAX_LINE);
  if (readLine(s_local, operation, MAX_LINE) == -1) {
    fprintf(stderr, "s> ERROR reading line\n");
    error = 2;
    pthread_exit(&error);
  }

  return_code = process_data(s_local, operation);

  send(s_local, &return_code, 1, MSG_NOSIGNAL);

  /* If we are sending a message, we also return to the sender the message id */
  if(strcmp(operation, "SEND\0") == 0 || strcmp(operation, "SENDATTACH\0") == 0){
    char id_toClient[MAX_LINE];
    sprintf(id_toClient, "%u", (next_message_id-1));
    // printf("ID: %s\n", id_toClient);
    sendToClient(s_local, id_toClient);
  }
  close(s_local);
  pthread_exit(0);

  return 0;
}



int connect_user(int s_local, char* username){

  struct sockaddr_in client_addr;
  socklen_t size;
  size = sizeof(struct sockaddr_in);
  getpeername(s_local, (struct sockaddr *)&client_addr, &size);

  struct Node *user_node;
  user_node = search(username);

  //User does not exist
  if(user_node == NULL){
    printf("s> CONNECT %s FAIL\n", username);
    return 1;
  }
  //User is already connected
  if(user_node->data->status != 0){
    printf("s> CONNECT %s FAIL\n", username);
    return 2;
  }
  else{ //User is disconnected

    //Filling the data for the user node to be connected
    struct user *data_connected = (struct user*) malloc(sizeof(struct user));
    strcpy(data_connected->username, username);
    data_connected->status = TRUE;
    data_connected->ip_address = client_addr.sin_addr.s_addr;

    char argument2[MAX_LINE];
    if (readLine(s_local, argument2, MAX_LINE) == -1) {
      fprintf(stderr, "s> ERROR reading line\n");
      return 2;
    }
    data_connected->port = atoi(argument2);
    data_connected->pending_messages = user_node->data->pending_messages;

    //Create the updated node with the data previously filled
    struct Node *user_connected = getNewNode(data_connected);
    //Update node
    modify(user_connected);

    if(isEmpty(user_connected->data->pending_messages) == 0){

      //Preparing the socket to provide results
      int sd;
      struct sockaddr_in receiver_client;

      char msg_id_in_char[11];
      char * send_message = "SEND_MESSAGE";

      while(isEmpty(user_connected->data->pending_messages) != 1){

        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd == -1) {
          /* fprintf(stderr, "%s\n", "s> Could not create socket");*/
          return 3;
        }

        bzero((char *)&receiver_client, sizeof(struct sockaddr_in));
        receiver_client.sin_addr.s_addr = user_connected->data->ip_address;
        // memcpy(&(receiver_client.sin_addr), user_connected->data->ip_address, sizeof(struct in_addr));
        receiver_client.sin_family = AF_INET;
        receiver_client.sin_port = htons(user_connected->data->port);

        if (connect(sd, (struct sockaddr *)&receiver_client, sizeof(struct sockaddr_in)) < 0) {
          return 3;
        }

        NODE *queue_message = Dequeue(user_connected->data->pending_messages);

        // sendToClient(SOCKET, queue_message->data.mes->text)
        //  sendToClient(user_connected->data->port, queue_message->data.mes->text, MAX_LINE);
        sprintf(msg_id_in_char, "%u", queue_message->data.mes->id);

        sendToClient(sd, send_message);
        sendToClient(sd, queue_message->data.mes->from_user);
        sendToClient(sd, msg_id_in_char);
        sendToClient(sd, queue_message->data.mes->text);

        close(sd);

        printf("s> SEND MESSAGE %d FROM %s TO %s\n", queue_message->data.mes->id, queue_message->data.mes->from_user, user_connected->data->username);
      }
    }
  }

  return 0;

}

int disconnect(char* username){
  //stop CONNECT thread

  Node* userNode = search(username);

  /* If the user is not found inside the data structure */
  if(userNode == NULL){
    printf("s> DISCONNECT %s FAIL\n", username);
    return 1;
  }

  /* If the user is not connected */
  if(userNode->data->status == FALSE){
    printf("s> DISCONNECT %s FAIL\n", username);
    return 2;
  }

  /* If the user is connected */
  if(userNode->data->status == TRUE){
    userNode->data->status = FALSE;
    userNode->data->ip_address = 0;
    userNode->data->port = 0;

    /* Modify the user node on the data structure */
    if(modify(userNode) < 0){
      printf("s> DISCONNECT %s FAIL\n", username);
      return 3;
    }
    printf("s> DISCONNECT %s OK\n", username);
    return 0;
  }

  /* Error case */
  printf("s> DISCONNECT %s FAIL\n", username);
  return 3;
}

int unregister(char* username){
  CLIENT* clnt;
  clnt = clnt_create("localhost", USERSTORAGE, USERSTORAGEVER, "tcp");
  if (clnt == NULL) {
    fprintf(stderr, "s> UNREGISTER %s FAIL\n", username);
    clnt_destroy(clnt);
    return 2;
  }
  int result = *unregister_user_1(username, clnt);
  if(result != 0) {
    fprintf(stderr, "s> UNREGISTER %s FAIL\n", username);
    clnt_destroy(clnt);
    return result;
  }
  /* Succesful unregister */
  if(delete(username) == 0){
    printf("s> UNREGISTER %s OK\n", username);
    clnt_destroy(clnt);
    return 0;
  }
  /* ERROR: user is not found in the data structure */
  else if(delete(username) == -1){
    fprintf(stderr, "s> UNREGISTER %s FAIL\n", username);
    clnt_destroy(clnt);
    return 1;
  }
  /* ERROR: any other case */
  else{
    fprintf(stderr, "s> UNREGISTER %s FAIL\n", username);
    clnt_destroy(clnt);
    return 2;
  }
  clnt_destroy(clnt);
}

int send_message(char* sender, char* receiver, char* message){

    CLIENT* clnt;
    clnt = clnt_create("localhost", USERSTORAGE, USERSTORAGEVER, "tcp");
    if (clnt == NULL) {
      fprintf(stderr, "s> SEND FAILED\n");
      return 2;
    }

    /* The message exceeds the maximum size */
    if((strlen(message)+1) > MAX_LINE){
      return 2;
    }

    /* Find the two users inside the list */
    Node* senderNode = search(sender);
    Node* receiverNode = search(receiver);

    /* If one of the users is not found inside the data structure */
    if((senderNode == NULL) || (receiverNode == NULL)){
      return 1;
    }

    /* If the sender is disconnected */
    if(senderNode->data->status == FALSE){
      return 2;
    }

    NODE *receiver_message = malloc(sizeof(NODE));
    receiver_message->data.mes = (struct message*)malloc(sizeof(struct message));
    pthread_mutex_lock(&message_id_lock);
    receiver_message->data.mes->id = next_message_id;
    /* If we exceed the maximum size for an unsigned int*/
    if(next_message_id + 1 > UINT_MAX){
      next_message_id = 0;
    } else {
      next_message_id++;
    }
    pthread_mutex_unlock(&message_id_lock);

    senderNode->data->last_message = receiver_message->data.mes->id;

    strcpy(receiver_message->data.mes->from_user, sender);
    strcpy(receiver_message->data.mes->to_user, receiver);
    strcpy(receiver_message->data.mes->text, message);

    // printf("Sender from: %s\n", receiver_message->data.mes->from_user);
    // printf("Sender to: %s\n", receiver_message->data.mes->to_user);
    // printf("Sender mes: %s\n", receiver_message->data.mes->text);

    /* If the receiver is disconnected */
    if(receiverNode->data->status == FALSE){
      /* Put the message in the message queue */
      Enqueue(search(receiver)->data->pending_messages, receiver_message);
      printf("s> MESSAGE %u FROM %s TO %s STORED\n", receiver_message->data.mes->id, sender, receiver);
      return 0;
    } else {

      int sd;
      struct sockaddr_in receiver_client;

      sd = socket(AF_INET, SOCK_STREAM, 0);
      if (sd == -1) {
        /* fprintf(stderr, "%s\n", "s> Could not create socket");*/
        return 3;
      }

      bzero((char *)&receiver_client, sizeof(struct sockaddr_in));
      uint32_t ip = receiverNode->data->ip_address;
      receiver_client.sin_addr.s_addr = ip;
      // memcpy(&(receiver_client.sin_addr), receiverNode->data->ip_address, sizeof(struct in_addr));
      receiver_client.sin_family = AF_INET;
      receiver_client.sin_port = htons(receiverNode->data->port);

      if (connect(sd, (struct sockaddr *)&receiver_client, sizeof(struct sockaddr_in)) < 0) {
        return 3;
      }

      char msg_id_in_char[11];
      char * send_message = "SEND_MESSAGE";
      sprintf(msg_id_in_char, "%u", receiver_message->data.mes->id);

      sendToClient(sd, send_message);
      sendToClient(sd, senderNode->data->username);
      sendToClient(sd, msg_id_in_char);
      sendToClient(sd, receiver_message->data.mes->text);

      int* result = add_message_1(*(receiver_message->data.mes), clnt);
      if (*result != 0) {
        fprintf(stderr, "s> STORE MESSAGE FAILED\n");
        return *result;
      }
      clnt_destroy(clnt);

      printf("s> SEND MESSAGE %d FROM %s TO %s\n", receiver_message->data.mes->id, sender, receiver);
    }
    return 0;
}

int sendAttach(char* sender, char* receiver, char* message, char* fileName, char* fileContent){
    /* The message exceeds the maximum size */
    if((strlen(message)+1) > MAX_LINE){
      return 2;
    }

    /* Find the two users inside the list */
    Node* senderNode = search(sender);
    Node* receiverNode = search(receiver);

    /* If one of the users is not found inside the data structure */
    if((senderNode == NULL) || (receiverNode == NULL)){
      return 1;
    }

    /* If the sender is disconnected */
    if(senderNode->data->status == FALSE){
      return 2;
    }

    // printf("SENDER: %s\n", sender);
    // printf("RECEIVER: %s\n", receiver);
    // printf("MESSAGE: %s\n", message);
    // printf("FILENAME: %s\n", fileName);
    // printf("FILE CONTENT: %s\n", fileContent);

    // Store the files associated with the messages on an independent storage server developed with RPC !!!!!!!!!!!!!

    NODE *receiver_message = malloc(sizeof(NODE));
    receiver_message->data.mes = (struct message*)malloc(sizeof(struct message));
    pthread_mutex_lock(&message_id_lock);
    receiver_message->data.mes->id = next_message_id;
    /* If we exceed the maximum size for an unsigned int*/
    if(next_message_id + 1 > UINT_MAX){
      next_message_id = 0;
    } else {
      next_message_id++;
    }
    pthread_mutex_unlock(&message_id_lock);

    senderNode->data->last_message = receiver_message->data.mes->id;

    strcpy(receiver_message->data.mes->from_user, sender);
    strcpy(receiver_message->data.mes->to_user, receiver);
    strcpy(receiver_message->data.mes->text, message);
    strcpy(receiver_message->data.mes->filename, fileName);
    //store fileContent

    /* If the receiver is disconnected */
    if(receiverNode->data->status == FALSE){
      /* Put the message in the message queue */
      Enqueue(search(receiver)->data->pending_messages, receiver_message);
      printf("s> MESSAGE %u FROM %s TO %s STORED\n", receiver_message->data.mes->id, sender, receiver);
      return 0;
    } else {

      int sd;
      struct sockaddr_in receiver_client;

      sd = socket(AF_INET, SOCK_STREAM, 0);
      if (sd == -1) {
        /* fprintf(stderr, "%s\n", "s> Could not create socket");*/
        return 3;
      }

      bzero((char *)&receiver_client, sizeof(struct sockaddr_in));
      receiver_client.sin_addr.s_addr = receiverNode->data->ip_address;
      // memcpy(&(receiver_client.sin_addr), receiverNode->data->ip_address, sizeof(struct in_addr));
      receiver_client.sin_family = AF_INET;
      receiver_client.sin_port = htons(receiverNode->data->port);

      if (connect(sd, (struct sockaddr *)&receiver_client, sizeof(struct sockaddr_in)) < 0) {
        return 3;
      }

      char msg_id_in_char[11];
      char * send_attach = "SEND_ATTACH";
      sprintf(msg_id_in_char, "%u", receiver_message->data.mes->id);

      sendToClient(sd, send_attach);
      sendToClient(sd, senderNode->data->username);
      sendToClient(sd, msg_id_in_char);
      sendToClient(sd, receiver_message->data.mes->text);
      sendToClient(sd, receiver_message->data.mes->filename);
      sendToClient(sd, fileContent);
      // printf("File content: %s\n", fileContent);

      printf("s> SEND ATTACH %d WITH FILE %s FROM %s TO %s\n", receiver_message->data.mes->id, receiver_message->data.mes->filename, sender, receiver);
    }
    return 0;
}


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
