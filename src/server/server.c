#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "message.h"
#include "request.h"
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#define FALSE 0
#define TRUE 1

#define LISTEN_BACKLOG 50

pthread_mutex_t mutex_msg;
pthread_cond_t cond_msg;
int sock_not_free = 1;

int server_socket;

int process_request(struct sockaddr_in* client, struct request* req);

int main(int argc, char* argv[]) {
    char* server_ip;
    int server_port;
    // int server_socket;
    struct sockaddr_in server, client;
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);

    // TODO: Check numeric input
    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
      fprintf(stderr, "%s\n\n", "usage: ./server -p <port>");
      return -1;
    }

    server_port = atoi(argv[2]);
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // Open socket

    if (server_socket == -1) {
      fprintf(stderr, "%s\n", "ERROR socket cannot be opened");
      return -1;
    }

    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET; // Set family IPv4
    //snprintf(ifr.ifr_name, IFNAMSIZ, "eth0");
    snprintf(ifr.ifr_name, IFNAMSIZ, "lo"); // TODO: Default network interface
    ioctl(server_socket, SIOCGIFADDR, &ifr);
    server_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    /**********************************************/

    server.sin_family = AF_INET;
    if (inet_aton(server_ip, &server.sin_addr) == 0) {
      fprintf(stderr, "%s\n", "ERROR invalid IP address");
      return -1;
    }
    server.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1) {
      fprintf(stderr, "%s%d\n", "ERROR binding failed");
      return -1;
    }

    if (listen(server_socket, LISTEN_BACKLOG) == -1) {
      fprintf(stderr, "%s\n", "ERROR listening failed");
      return -1;
    }

    pthread_attr_t t_attr;
    pthread_mutex_init(&mutex_msg, NULL);
    pthread_cond_init(&cond_msg, NULL);
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    pthread_t thr;

    printf("s> init server %s:%d\n", server_ip, server_port);

    while(TRUE) {
      printf("s> ");
      printf("vale\n");

      accept(server_socket, (struct sockaddr *)&client, &peer_addr_size);
      pthread_create(&thr, &t_attr, (void*)process_request, &client);

      pthread_mutex_lock(&mutex_msg);
      while(sock_not_free) {
        pthread_cond_wait(&cond_msg, &mutex_msg);
      }
      sock_not_free = TRUE;
      pthread_mutex_unlock(&mutex_msg);
    }
}

int process_data(struct request* req) {

  return 0;
}

int process_request(struct sockaddr_in* client, struct request* req) {
  struct sockaddr_in local_client;
  struct request local_req;
  int dedicated_sock;

  pthread_mutex_lock(&mutex_msg);
  memcpy(&local_client, &client, sizeof(struct sockaddr_in));
  read(server_socket, &local_req, sizeof(struct request));

  sock_not_free = FALSE;
  pthread_cond_signal(&cond_msg);

  pthread_mutex_unlock(&mutex_msg);

  int return_code = process_data(&local_req);

  dedicated_sock = socket(AF_INET, SOCK_STREAM, 0); // Open socket

  return 0;
}
