program USERSTORAGE {
  version USERSTORAGEVER {
    int init() = 1;
    int register_user(struct user new_user) = 2;
    int unregister_user(char* username)     = 3;
    int get_user(struct user* usr)          = 4;
    int add_message(struct message msg)     = 5;
    int get_total_messages(char* username)  = 6;
    int get_message(char* username, unsigned int msg_id, ) = 7; // TODO: AQUI HAY QUE METER MD5
  } = 1;
} = 98;
