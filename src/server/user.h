struct user {
    char[256] username;
    int status;
    unsigned long ip_addr;
    int port;
    queue* pending_messages;
    int last_message;
};
