  #include "mensaje.h"
  #include "server.h"
  #include "doubleLinkedList.c"

  #define PRINTEA 0 //If it is 1 the messages are printed


  /*It determines which is the action to be executed
  It returns 0 in success and -1 on error*/
  int chooseAction(struct request *msg_local){
    /*I check all the possible calls from the user*/
    if(strcmp(msg_local -> action_name, INIT) == 0){
      printf("%s\n", INIT);
      if(init() < 0){
        perror("Error in init\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,SET) == 0){
      printf("%s\n",SET);
      if(set_value(msg_local -> key, msg_local -> value1, msg_local -> value2) < 0){
        perror("Error in set_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,GET) == 0){
      printf("%s\n", GET);
      if(get_value(msg_local -> key, msg_local -> value1,  &(msg_local -> value2)) < 0){
        perror("Error in get_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,MODIFY) == 0){
      printf("%s\n", MODIFY);
      if(modify_value(msg_local -> key, msg_local -> value1, &(msg_local -> value2)) < 0){
        perror("Error in modify_value\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,DELETE) == 0){
      printf("%s\n", DELETE);
      if(delete_key(msg_local -> key) < 0){
        perror("Error in delete_key\n");
        return -1;
      }
    }
    else if (strcmp(msg_local -> action_name,NUM) == 0){
      printf("%s\n", NUM);
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
      return -1;
    }
    memcpy((char *) &msg_local, msg, sizeof(struct request));

    /* wake up server */
    msg_not_copied = FALSE; /* FALSE = 0 */
    if (pthread_cond_signal(&cond_msg) != 0) {
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t lock the mutex");
      return -1;
    }
    if (pthread_mutex_unlock(&mutex_msg) != 0) {
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t lock the mutex\n");
      return -1;
    }

    if(chooseAction(&msg_local) < 0){ /*I choose the action that has to be executed*/
      perror("Can’t locate the action to be executed\n");
      return -1;
    }
    if(send(0,&msg_local) < 0){ /*I send the response to the client*/
      send(-1, &msg_local); /*send an error signal to the client */
      perror("Can’t send the response\n");
      return -1;
    }
    displayForward();
    printf("\n\n");
    pthread_exit(0);
  }

  /*It allows the initialization of the system
  All the triplets stored in the system are destroyed
  It returns 0 in success and -1 on error*/
  static int init(){
    while(!isEmpty()){//I executed the loop until the list is empty
      deleteFirst();//I delete the first element
    }
    return 0;
  }

  /*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
  static int set_value(int key, char *value1, float value2){
    if(contain(key)) return -1;//There is already a triplet with the same key
    else if(length() > 0) insertLast(key, value1, value2);//If the list is not empty I store it at the end
    else insertFirst(key, value1, value2);//As the list is empty I store it at the beginning
    return 0;
  }

  /*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
  static int get_value(int key, char *value1, float *value2){
    if(!contain(key)) return -1;//The triplet does not exist
    if(!obtainElement(key, value1, value2)){//I get the values of the triplets
      return -1;
    }
    return 0;
  }

  /*Modifies the values associated with a key
  It returns 0 in success and -1 on error
  Not existing an element with that key is considered an error*/
  static int modify_value(int key, char *value1, float *value2){
    if(!contain(key)) return -1;//The triplet does not exist
    if(!changeValues(key, value1, value2)){//I change the values of the triplet
      return -1;
    }
    return 0;
  }

  /*It deletes an element
  Not existing an element with that key is considered an error*/
  static int delete_key(int key){
    if(!contain(key)) return -1;//The triplet does not exist
    if(delete(key) == NULL){//I delete the triplet
      return -1;
    }
    return 0;
  }

  /*It stores the number of elements stored
  It returns 0 in success and -1 on error*/
  static int num_items(){
    return length();
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
    if(pthread_mutex_init(&mutex_msg, NULL) != 0){  /*mutex*/
      perror("Can’t initialize the mutex\n");
      return -1;
    }
    if(pthread_cond_init(&cond_msg, NULL) != 0){ /*conditional variable*/
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




  /*Auxiliary method to fill the list with trivial element*/
  void fillList(){
       insertFirst(1,"Uno",10.0);
       insertLast(2,"Dos",20.0);
       insertLast(3,"Tres",30.0);
       insertLast(4,"Cuatro",1.0);
       insertLast(5,"Cinco",40.0);
       insertLast(6,"Seis",56.0);
       if(PRINTEA)displayForward();
  }

  /*It tests the init method of the server
    It returns 0 in success and -1 on error*/
  int testInit(){
    fillList();//I fill the list with trivial elements
    init();//I initialize the list so the list should be empty
    if(isEmpty()){
      printf("The list is empty (CORRECT)\n");
      return 0;
    }
    else {
      printf("ERROR testInit\n");
      return-1;
    }}

  /*It test the contain method
    It returns 0 in success and -1 on error*/
  int testContain(){
    fillList();//I fill the list with trivial elements
    if(contain(1)){
      printf("It contains 1 (CORRECT)\n");
      return 0;
    }
    else {
      printf("ERROR testContain\n");
      return-1;
    }
  }

  /*It test the set_value method
  It returns 0 in success and -1 on error*/
  int testSet_value(){
    init();//I initialize the list so the list should be empty
    if(set_value(1, "One", 1.0) < 0){//An error while setting a triplet
      printf("ERROR testSet_value\n");
      return -1;
    }
    if(contain(1)){//I check if the list contains the element I have just introduced
      printf("It contains 1 (CORRECT)\n");
      return 0;
    }
    else{
      printf("ERROR testSet_value\n");
      return-1;
    }
  }

  /*It test the get_value method
  It returns 0 in success and -1 on error*/
  int testGet_value(){
    init();//I initialize the list so the list should be empty
    fillList();//I fill the list with trivial elements
    char value1[256];//Auxiliary variables
    float value2 = 2.0;
    if(get_value(1, value1, &value2) < 0){
      printf("ERROR testGet_value\n");
      return-1;
    }
    else if(value2 == 10.0 && strcmp(value1,"Uno\n")){
      printf("It contains the correct values %f and %s (CORRECT)\n", value2, value1);
      return 0;
    }
    else{
      printf("ERROR testGet_value\n");
      return-1;
    }
  }

  /*It test the get_value method
  It returns 0 in success and -1 on error*/
  int testModify_value(){
    init();//I initialize the list so the list should be empty
    fillList();//I fill the list with trivial elements
    float value2 = 99.0;
    if(modify_value(1, "New", &value2) < 0){//I modify the value of 1
      printf("ERROR testModify_value\n");
      return-1;
    }
    else{
      value2 = 35.0;//I change the original value of the variable to make sure there is no problem with its pointer
      float value3 = 0.0;//Auxiliary variable
      char value1[256];
      if(get_value(1, value1, &value3) < 0){
        printf("ERROR testModify_value\n");
        return-1;
      }
      else if(value3 == 99.0 && strcmp(value1,"New\n")){
        printf("Values modified correctly (CORRECT)\n");
        return 0;
      }
      else{
        printf("ERROR testModify_value %s %f\n", value1, value3);
        return-1;
      }
    }
  }

  /*It test the delete_key method
  It returns 0 in success and -1 on error*/
  int testDelete_key(){
    init();//I initialize the list so the list should be empty
    fillList();//I fill the list with trivial elements
    int oldLength = length(), newLength;//Auxiliary variables
    if(delete_key(1) < 0){//I delete the first element
      printf("ERROR testDelete_key\n");
      return-1;
    }
    newLength = length();
    if( (oldLength -1) != newLength){//I check if the length is correct
      printf("ERROR testDelete_key length\n");
      return-1;
    }
    if(contain(1)){//I check if the list contains the element deleted
      printf("ERROR testDelete_key contain\n");
      return-1;
    }
    printf("Triplet deleted correctly (CORRECT)\n");
    return 0;
  }

  /*It test the num_items method
  It returns 0 in success and -1 on error*/
  int testNum_items(){
    init();//I initialize the list so the list should be empty
    fillList();//I fill the list with trivial elements
    if(num_items() != 6){
      printf("ERROR testNum_items\n");
      return-1;
    }
    printf("Correct number of items (CORRECT)\n");
    return 0;
  }

  /*I run all the tests
  It returns 0 in success and -1 if any test fails*/
  int testAll(){
    int check = 0;//Variable to know if any test fails
    check =- testInit();
    check =- testContain();
    check =- testSet_value();
    check =- testGet_value();
    check =- testModify_value();
    check =- testDelete_key();
    check =- testNum_items();
    return check;
  }
