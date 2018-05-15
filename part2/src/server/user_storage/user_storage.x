struct DATA {
    struct message* mes;
};

struct NODE {
   DATA data;
   struct NODE *prev;
};

struct Queue {
    NODE *head;
    NODE *tail;
    int size;
    int limit;
};

struct message {
    unsigned int id;
    char md5[32];
    char from_user[256];
    char to_user[256];
    char text[256];
};

struct user {
    char username[256];
    int status;
    int ip_address;
    int port;
    Queue* pending_messages;
    unsigned int last_message;
};

program USERSTORAGE {
  version USERSTORAGEVER {
    int init() = 1;
    int register_user(string username) = 2;
    int unregister_user(string username)   = 3;
    int add_user(struct user usr) = 4;
    struct user get_user(string username) = 5;
    int add_message(struct message msg)     = 6;
    int get_total_messages(string username)  = 7;
    struct message get_message(string username, unsigned int msg_id) = 8;
  } = 1;
} = 98;
