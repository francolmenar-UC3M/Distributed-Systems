#ifndef MESSAGE_H
#define MESSAGE_H

struct message {
    unsigned int id;
    char md5[32];
    char from_user[256];
    char to_user[256];
    char text[256];
};

#endif /* MESSAGE_H */
