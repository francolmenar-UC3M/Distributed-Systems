#include "mensaje.h"
#include "keys.h"

/*It locks the muex associated to the list's operations
  It checks the global variable too for avoiding false signals*/
static int lockMutex(){
  if (pthread_mutex_lock(&mutex_id) != 0) {
          perror("Can’t lock the mutex");
          return -1;
  }
  while (id_not_copied) /*waiting while the list is being used*/
          pthread_cond_wait(&cond_id, &mutex_id);
  id_not_copied = TRUE; /* TRUE = 1 */
  return 0;
}

/*It unlocks the mutex associated to the list's operations
  It changes the global variable too*/
static int unlockMutex(){
  id_not_copied = FALSE; /* FALSE = 0 */
  if (pthread_cond_signal(&cond_id) != 0) { /*send the signal*/
    perror("Can’t lock the mutex");
    return -1;
  }
  if (pthread_mutex_unlock(&mutex_id) != 0) { /*unlock the mutex*/
    perror("Can’t lock the mutex\n");
    return -1;
  }
  return 0;
}


/*It configures the queues in order to being able to use them*/
static int set_up(){
    attr.mq_maxmsg = 20; /*Atributes of the queue of the client side*/
    attr.mq_msgsize = sizeof(struct response);
    /*lockMutex();
    char aux_name [TOP_EXP_CLIENT];
    strcpy(req.q_name,  CLIENT); /copy the base name/
    snprintf(aux_name, TOP_EXP_CLIENT, "%d", id);
    strcat(req.q_name, aux_name);
    id++;
    unlockMutex();
  //printf("Name %s\n", req.q_name);*/
    q_client = mq_open(CLIENT,  O_CREAT|O_RDONLY,  0700,  &attr); /*client queue*/
    if(q_client == -1){//check for errors while opening the queues
      perror("Can’t open client queue ");
      return -1;
    }
    q_server = mq_open(SERVER,  O_WRONLY); /*server queue*/
    if(q_server == -1){
      perror("Can’t open client queue ");
      return -1;
    }
    return 0;
}

/*It sends the request to the server
  It waits for its response
  It stores the response*/
static int send(){
  /*I send a request to the server through the server queue*/
  if(mq_send(q_server, (char *) &req, sizeof(struct request), 0) < 0){
    perror("Can’t send the request to the server\n");
    return -1;
  }
  /*I receive the respond of the server by the client queue*/
  if(mq_receive(q_client, (char *) &res, sizeof(struct response), 0) < 0){
    perror("Can’t receive server request\n");
    return -1;
  }
  if(mq_close(q_server) < 0){
    perror("Can’t close the server queue\n");
    return -1;
  }
  if(mq_close(q_client) < 0){
    perror("Can’t close the client queue\n");
    return -1;
  }
  if(mq_unlink(CLIENT) < 0){
    perror("Can’t unlink the queue\n");
    return -1;
  }
  return 0;
}

/*It allows the initialization of the system
All the triplets stored in the system are destroyed
It returns 0 in success and -1 on error*/
static int init (){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  strcpy(req.action_name,  INIT);
  strcpy(req.q_name,  CLIENT);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side of init\n");
    return -1;
  }
  return 0;
}

/*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
static int set_value(int key, char *value1, float value2){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = value2;
  strcpy(req.q_name,  CLIENT);
  strcpy(req.action_name,  SET);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side of set_value\n");
    return -1;
  }
  return 0;
  }

/*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
static int get_value(int key, char *value1, float *value2){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = *value2;
  strcpy(req.action_name,  GET);
  strcpy(req.q_name,  CLIENT);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side of get_value\n");
    return -1;
  }
  strcpy(value1, res.value1); /*I copy the result*/
  float aux = res.value2;
  *value2 = aux;
  return 0;
  }

/*Modifies the values associated with a key
  It returns 0 in success and -1 on error
  Not existing an element with that key is considered an error*/
static int modify_value(int key, char *value1, float *value2){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = *value2;
  strcpy(req.action_name,  MODIFY);
  strcpy(req.q_name,  CLIENT);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side of modify_value\n");
    return -1;
  }
  return 0;
}

/*It deletes an element
Not existing an element with that key is considered an error*/
static int delete_key(int key){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.action_name,  DELETE);
  strcpy(req.q_name,  CLIENT);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side of delete\n");
    return -1;
  }
  return 0;
}

/*It stores the number of elements stored
It returns 0 in success and -1 on error*/
static int num_items(){
  if(set_up() < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  strcpy(req.action_name,  NUM);
  if(send()  == -1){
    perror("Can’t send the request\n");
    return -1;
  }
  if(res.error < 0){/*I check for errors*/
    perror("Error in the server side num_items\n");
    return -1;
  }
  return (int) (res.value2); /*I return the result as an integer*/
}
