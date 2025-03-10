  #include <mqueue.h>
  #include <pthread.h>
  #include <stdio.h>
  #include <string.h>
  #include <sys/types.h>
  #include <stdio.h>
  #include <stdlib.h>

  #define TRUE 1
  #define FALSE 0
  #define TOP_EXP_CLIENT 5
  #define NAME_SIZE 6 /*length of the client of the name*/

  mqd_t q_server = -1; /* server message queue */

  struct request req; /*request to the server*/
  struct response res; /*respond from the server*/
  struct mq_attr attr; /*queue atributes*/

  /*It closes the server queue*/
  static int closeServer();

  /*It closes and unlink the client queue*/
  static int closeClient(int q_descriptor, char * client);

  /*It creates a random name for the client queue*/
  static int rand_string(char * client);

  /*It configures the queues in order to being able to use them
    It creates the client queue identifiers*/
  static int set_up(char * client);

  /*It sends the request to the server
    Recieve as an input the client queue descriptor
    It waits for its response
    It stores the response*/
  static int send(int q_descriptor, char * client);

  /*It allows the initialization of the system
  All the triplets stored in the system are destroyed
  It returns 0 in success and -1 on error*/
  static int init();

  /*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
  static int set_value(int key, char *value1, float value2);

  /*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
  static int get_value(int key, char *value1, float *value2);

  /*Modifies the values associated with a key
  It returns 0 in success and -1 on error
  Not existing an element with that key is considered an error*/
  static int modify_value(int key, char *value1, float *value2);

  /*It deletes an element
  Not existing an element with that key is considered an error*/
  static int delete_key(int key);

  /*It stores the number of elements stored
  It returns 0 in success and -1 on error*/
  static int num_items();
