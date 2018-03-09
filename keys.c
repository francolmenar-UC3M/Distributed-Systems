#include "mensaje.h"
#include "keys.h"

/*It closes the server queue*/
static int closeServer(){
  if(mq_close(q_server) < 0){ /*close*/
    perror("Can’t close the server queue\n");
    return -1;
  }
  return 0;
}

/*It closes and unlink the client queue*/
static int closeClient(int q_descriptor, char * client){
  if(mq_close(q_descriptor) < 0){ /*close*/
    perror("Can’t close the client queue\n");
    return -1;
  }
  if(mq_unlink(client) < 0){ /*unlink*/
    perror("Can’t unlink the queue\n");
    return -1;
  }
  return 0;
}

/*It creates a random name for the client queue*/
static int rand_string(char * client){
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int size = NAME_SIZE; /*I copy the size of the name to an auxiliary variable*/
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            client[n] = charset[key];
        }
        client[size] = '\0';
        client[0] = '/';
    }
    return 0;
}

/*It configures the queues in order to being able to use them
  It creates the client queue identifiers*/
static int set_up(char * client){
    int q_descriptor; /*client queue descriptor*/
    attr.mq_maxmsg = 20; /*Atributes of the queue of the client side*/
    attr.mq_msgsize = sizeof(struct response);
    q_descriptor = mq_open(client,  O_CREAT|O_RDONLY,  0700,  &attr); /*client queue*/
    if(q_descriptor == -1){ /*check for errors while opening the queues*/
      perror("Can’t open client queue ");
      return -1;
    }
    if (q_server == -1){ /*check to not reopen the server queue*/
      if(atexit((void *) (* closeServer)) != 0){ /*set the exit function*/
        perror("Can’t set the exit function");
        return -1;
      }
      q_server = mq_open(SERVER,  O_WRONLY); /*server queue*/
    }
    if(q_server == -1){
      perror("Can’t open client queue ");
      return -1;
    }
    return q_descriptor;
}

/*It sends the request to the server
  Recieve as an input the client queue descriptor
  It waits for its response
  It stores the response*/
static int send(int q_descriptor, char * client){
  /*I send a request to the server through the server queue*/
  if(mq_send(q_server, (char *) &req, sizeof(struct request), 0) < 0){
    perror("Can’t send the request to the server\n");
    return -1;
  }
  /*I receive the respond of the server by the client queue*/
  if(mq_receive(q_descriptor, (char *) &res, sizeof(struct response), 0) < 0){
    perror("Can’t receive server request\n");
    return -1;
  }
  return closeClient(q_descriptor, client); /*close the client queue*/
}

/*It allows the initialization of the system
All the triplets stored in the system are destroyed
It returns 0 in success and -1 on error*/
static int init (){
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if((q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  strcpy(req.action_name,  INIT); /*copy the function name*/
  strcpy(req.q_name,  client); /*copy the client queue's name*/
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
    return -1;
  }
  if(res.error < 0){ /*I check for errors*/
    perror("Error in the server side of init\n");
    return -1;
  }
  return 0;
}

/*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
static int set_value(int key, char *value1, float value2){
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if( (q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = value2;
  strcpy(req.q_name,  client); /*copy the client queue's name*/
  strcpy(req.action_name,  SET); /*copy the function name*/
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
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
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if( (q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = *value2;
  strcpy(req.action_name,  GET);
  strcpy(req.q_name,  client);
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
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
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if( (q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.value1,  value1);
  req.value2 = *value2;
  strcpy(req.action_name,  MODIFY);
  strcpy(req.q_name,  client);
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
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
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if( (q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  req.key = key; /*I copy the variables to the request struc*/
  strcpy(req.action_name,  DELETE);
  strcpy(req.q_name,  client);
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
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
  int q_descriptor; /*client queue descriptor*/
  char client [NAME_SIZE]; /*the name of the client queue*/
  rand_string(client); /*create the random name of the client queue*/
  if( (q_descriptor = set_up(client)) < 0){
    perror("Can’t set up the queues\n");
    return -1;
  }
  strcpy(req.q_name,  client);
  strcpy(req.action_name,  NUM);
  if(send(q_descriptor, client)  == -1){
    perror("Can’t send the request\n");
    closeClient(q_descriptor, client); /*close the client queue*/
    return -1;
  }
  if(res.error < 0){ /*I check for errors*/
    perror("Error in the server side num_items\n");
    return -1;
  }
  return (int) (res.value2); /*I return the result as an integer*/
}
