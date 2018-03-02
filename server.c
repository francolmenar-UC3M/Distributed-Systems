  #include "mensaje.h"
  #include "server.h"
  #include "doubleLinkedList.c"


  /*It determines which is the action to be executed
  It returns 0 in success and -1 on error*/
  int chooseAction(struct request *msg_local){
    /*I check all the possible calls from the user*/
    if(strcmp(msg_local -> action_name, INIT) == 0){
      if(init() < 0){
        perror("Error in init\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,SET) == 0){
      if(set_value(msg_local -> key, msg_local -> value1, msg_local -> value2) < 0){
        perror("Error in set_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,GET) == 0){
      if(get_value(msg_local -> key, msg_local -> value1,  &(msg_local -> value2)) < 0){
        perror("Error in get_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,MODIFY) == 0){
      if(modify_value(msg_local -> key, msg_local -> value1, &(msg_local -> value2)) < 0){
        perror("Error in modify_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,DELETE) == 0){
      if(delete_key(msg_local -> key) < 0){
        perror("Error in delete_key\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,NUM) == 0){
      if((msg_local -> key  = num_items()) < 0){
        perror("Error in num_items\n");
        return -1;
      }
      msg_local -> value2 = (float)(msg_local -> key); /*I cast the integer result to a float*/
    }
    else{
      perror("Can’t locate the action to be executed\n");
      return -1;
    }
    return 0;
  }

  /*It sends the response to the client*/
  int send(int error, struct request *msg_local){
    mqd_t q_client;/* client queue */

    /* return result to client by sending it to queue */
    q_client = mq_open(msg_local -> q_name, O_WRONLY);
    if (q_client == -1) {//check for errors while opening the queue
            perror("Can’t open client queue ");
            return -1;
    }
    else { /*respond*/
      struct response result;
      if(error < 0){ /*Error response*/
        result.error = error;
      }
      else{ /*Normal response*/
        result.error = 0;
        strcpy(result.value1, msg_local -> value1);
        result.value2 = msg_local -> value2;
      }
      if(mq_send(q_client, (const char *) &result, sizeof(struct response), 0) < 0){ /*I send the respond */
        perror("Can’t send the respond to the client\n");
        return -1;
      }
      if(mq_close(q_client) < 0){ /*I close the queue*/
        perror("Can’t close the client queue\n");
        return -1;
      }

    }
    return 0;
  }

  /*It process the request calling the desired method
  It returns 0 in success and -1 on error*/
  int request_process(struct request *msg){
    struct request msg_local;/* local message */

    /* thread copies message to local message*/
    if (pthread_mutex_lock(&mutex_msg) != 0) {
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t lock the mutex");
      pthread_exit(0);
      return -1;
    }
    memcpy((char *) &msg_local, msg, sizeof(struct request));

    /* wake up server */
    msg_not_copied = FALSE; /* FALSE = 0 */
    if (pthread_cond_signal(&cond_msg) != 0) {
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t lock the mutex");
      pthread_exit(0);
      return -1;
    }
    if (pthread_mutex_unlock(&mutex_msg) != 0) {
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t lock the mutex\n");
      pthread_exit(0);
      return -1;
    }

    if(chooseAction(&msg_local) < 0){ /*I choose the action that has to be executed*/
      send(-1, &msg_local); /*send an error signal to the client */
      pthread_exit(0);
      return -1;
    }
    if(send(0,&msg_local) < 0){ /*I send the response to the client*/
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t send the response\n");
      pthread_exit(0);
      return -1;
    }
    displayForward();
    printf("\n\n");
    pthread_exit(0);
  }

  /*It locks the muex associated to the list's operations
    It checks the global variable too for avoiding false signals*/
  static int lockMutex(){
    if (pthread_mutex_lock(&mutex_list) != 0) {
            perror("Can’t lock the mutex");
            return -1;
    }
    while (list_not_finished) /*waiting while the list is being used*/
            pthread_cond_wait(&cond_list, &mutex_list);
    list_not_finished = TRUE; /* TRUE = 1 */
    return 0;
  }

  /*It unlocks the mutex associated to the list's operations
    It changes the global variable too*/
  static int unlockMutex(){
    list_not_finished = FALSE; /* FALSE = 0 */
    if (pthread_cond_signal(&cond_list) != 0) { /*send the signal*/
      perror("Can’t lock the mutex");
      return -1;
    }
    if (pthread_mutex_unlock(&mutex_list) != 0) { /*unlock the mutex*/
      perror("Can’t lock the mutex\n");
      return -1;
    }
    return 0;
  }

  /*It allows the initialization of the system
  All the triplets stored in the system are destroyed
  It returns 0 in success and -1 on error*/
  static int init(){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    while(!isEmpty()){//I executed the loop until the list is empty
      deleteFirst();//I delete the first element
    }
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return 0;
  }

  /*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
  static int set_value(int key, char *value1, float value2){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    if(contain(key)){
      unlockMutex();
      return -1;//There is already a triplet with the same key
    }
    else if(length() > 0){
      insertLast(key, value1, value2);//If the list is not empty I store it at the end
    }
    else {
      insertFirst(key, value1, value2);//As the list is empty I store it at the beginning
    }
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return 0;
  }

  /*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
  static int get_value(int key, char *value1, float *value2){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    if(!contain(key)){
      unlockMutex();
      return -1;//The triplet does not exist
    }
    if(!obtainElement(key, value1, value2)){//I get the values of the triplets
      unlockMutex();
      return -1;
    }
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return 0;
  }

  /*Modifies the values associated with a key
  It returns 0 in success and -1 on error
  Not existing an element with that key is considered an error*/
  static int modify_value(int key, char *value1, float *value2){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    if(!contain(key)){
      unlockMutex();
      return -1;//The triplet does not exist
    }
    if(!changeValues(key, value1, value2)){//I change the values of the triplet
      unlockMutex();
      return -1;
    }
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return 0;
  }

  /*It deletes an element
  Not existing an element with that key is considered an error*/
  static int delete_key(int key){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    if(!contain(key)){
      unlockMutex();
      return -1;//The triplet does not exist
    }
    if(delete(key) == NULL){//I delete the triplet
      unlockMutex();
      return -1;
    }
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return 0;
  }

  /*It stores the number of elements stored
  It returns 0 in success and -1 on error*/
  static int num_items(){
    if(lockMutex() < 0) return -1; /*I lock the mutex*/
    int aux = length();
    if(unlockMutex() < 0) return -1; /*I unlock the mutex*/
    return aux;
  }

  int main(void){
    mqd_t q_server;            /*server queue*/
    struct request msg;        /*message to receive*/
    struct mq_attr q_attr;     /*queue atributes*/
    pthread_attr_t t_attr;     /*thread atributes*/
    pthread_t thid;

    q_attr.mq_maxmsg = 20;      /*maximum number of messages in queue*/
    q_attr.mq_msgsize = sizeof(struct request); /*maximum message size*/

    q_server = mq_open(SERVER,  O_CREAT|O_RDONLY,  0700,  &q_attr); /*open the queue*/
    if (q_server == -1) {
            perror("Can’t create server queue");
            return -1;
    }
    if(pthread_mutex_init(&mutex_msg, NULL) != 0){  /*message mutex*/
      perror("Can’t initialize the mutex\n");
      return -1;
    }
    if(pthread_mutex_init(&mutex_list, NULL) != 0){  /*list mutex*/
      perror("Can’t initialize the mutex\n");
      return -1;
    }
    if(pthread_cond_init(&cond_msg, NULL) != 0){ /*conditional variable*/
      perror("Can’t initialize the condition variable\n");
      return -1;
    }
    if(pthread_cond_init(&cond_list, NULL) != 0){ /*conditional variable*/
      perror("Can’t initialize the condition variable\n");
      return -1;
    }
    if(pthread_attr_init(&t_attr) != 0){ /*thread's attribute*/
      perror("Can’t initialize the attributes\n");
      return -1;
    }
    if(pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED) != 0){ /*detached thread*/
      perror("Can’t set the thread as detachable\n");
      return -1;
    }
    while (TRUE) {
      if(mq_receive(q_server, (char *) &msg, sizeof(struct request), 0) < 0){ /*I receive a request*/
        perror("Can’t receive client requests\n");
        return -1;
      }
      if(pthread_create(&thid, &t_attr,  (void *) (*request_process), &msg) != 0){ /*I create a thread*/
        perror("Can’t create the thread\n");
        return -1;
      }
      /* wait for thread to copy message */
      if (pthread_mutex_lock(&mutex_msg) != 0) {
              perror("Can’t lock the mutex");
              return -1;
      }
      while (msg_not_copied) /*waiting while the message is being copied*/
              pthread_cond_wait(&cond_msg, &mutex_msg);
      msg_not_copied = TRUE; /*I can wait for another request*/
      if (pthread_mutex_unlock(&mutex_msg) != 0) {
              perror("Can’t unlock the mutex");
              return -1;
      }
    }
    return 0;
  }
