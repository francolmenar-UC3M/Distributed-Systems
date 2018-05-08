#include <netinet/in.h>

struct user {
    char username[256];
    int status;
    struct in_addr* ip_address;
    int port;
    // queue* pending_messages;
    unsigned int last_message;
};
