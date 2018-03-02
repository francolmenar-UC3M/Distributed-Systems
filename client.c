#include "mensaje.h"
#include "keys.c"

#define NCLIENTS 2

static int normalExe(){
  printf("I set 1, 2 and 3\n");
  set_value(1, "Uno", 1.0);/*
  set_value(2, "Dos", 2.0);
  set_value(3, "Tres", 3.0);
  char aux [MAXSIZE];
  float aux2 = 40.0;
  printf("I get the values of 2\n");
  get_value(2,aux, &aux2);
  printf("Values of 2: value1 = %s\tvalue2 = %f\n",aux, aux2);
  aux2 = 4;
  printf("I modify the values of 3\n");
  modify_value(3, aux, &aux2);
  printf("I get the modified values\n");
  get_value(3,aux, &aux2);
  printf("Values of 3 after modification: value1 = %s\tvalue2 = %f\n",aux, aux2);
  printf("I delete 1\n");
  delete_key(1);
  int n = num_items();
  printf("Number of elements: %i\n", n);*/
  return 0;
}

int main(void) {
  pthread_attr_t t_attr;     /*thread atributes*/
  pthread_t clients;
  if(pthread_attr_init(&t_attr) != 0){ /*thread's attribute*/
    perror("Can’t initialize the attributes\n");
    return -1;
  }
  if(pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED) != 0){ /*detached thread*/
    perror("Can’t set the thread as detachable\n");
    return -1;
  }
  for(int i = 0; i < NCLIENTS; i++){
    if(pthread_create(&clients, &t_attr,  (void *) (*normalExe), NULL) != 0){ /*I create a thread*/
        perror("Can’t create the thread\n");
        return -1;
    }
  }
  while(1){

  }
  return 0;
}
