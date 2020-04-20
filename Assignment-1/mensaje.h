#ifndef COMMON_H_
#define COMMON_H_
#define MAXSIZE 256

#define SERVER "/Reverte"

#define INIT "init"
#define SET "set value"
#define GET "get value"
#define MODIFY "modify"
#define DELETE "delete"
#define NUM "num item"


struct request {
        int key; /*The key*/

        char value1 [MAXSIZE];
        float value2;

        char action_name[MAXSIZE]; /*The name of the action to perform*/

        char q_name[MAXSIZE]; /* client queue name – this is
                                where the server sends the reply
                                to */
};

struct response {
        int error; /*0 if there is no error and -1 otherwise*/
        char value1 [MAXSIZE];
        float value2;
};


#endif /* #ifndef COMMON_H_ */
