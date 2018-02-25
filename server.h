  #include <mqueue.h>
  #include <pthread.h>
  #include <stdio.h>
  #include <string.h>
  #include <sys/types.h>
  #include <stdio.h>


  #define TRUE 1
  #define FALSE 0

  /* mutex and condition variables for the message copy */
  pthread_mutex_t mutex_msg;
  int msg_not_copied = TRUE;        /* TRUE = 1 */
  pthread_cond_t cond_msg;

  /*It determines which is the action to be executed
  It returns 0 in success and -1 on error*/
  int chooseAction(struct request *msg_local);

  /*It sends the response to the client*/
  int send(int error, struct request *msg_local);

  /*It process the request calling the desired method
  It returns 0 in success and -1 on error*/
  int request_process(struct request *msg);

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
