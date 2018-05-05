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
#include <netdb.h>

#include <string.h>

#define FALSE 0
#define TRUE 1

// pthread_mutex_t mutex_msg;
// pthread_cond_t cond_msg;

int main(int argc, char* argv[]) {
    char* server_ip;
    int server_port;
    int server_socket;

    struct ifreq ifr;

    // TODO: Check numeric input
    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
      fprintf(stderr, "%s\n\n", "usage: ./server -p <port>");
      return -1;
    }

    server_port = atoi(argv[2]);
    server_socket = socket(AF_INET, SOCK_DGRAM, 0); // Open socket

    ifr.ifr_addr.sa_family = AF_INET; // Set family
    //snprintf(ifr.ifr_name, IFNAMSIZ, "eth0");
    snprintf(ifr.ifr_name, IFNAMSIZ, "lo"); // TODO: Default network interface
    ioctl(server_socket, SIOCGIFADDR, &ifr);
    server_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    /**********************************************/

    printf("s> init server %s:%d\n", server_ip, server_port);

    printf("s> ");

    struct message msg;
    pthread_attr_t t_attr;

    while(TRUE) {
      printf("kelok\n");
      for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 30000; j++) {
          /* code */
        }
      }
    }
}
