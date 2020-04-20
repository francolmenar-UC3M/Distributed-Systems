#include <netinet/in.h>
#include "queue.c"

struct user {
    char username[256];
    int status;
    struct in_addr* ip_address;
    int port;
    Queue* pending_messages;
    unsigned int last_message;
};
