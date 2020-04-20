#include "mensaje.h"
#include "keys.c"
#include <unistd.h>

#define NCLIENTS 1 /*number f threads to be created*/

/*It simulates a common execution of a client*/
static int normalExe(){
  if( (init()) < 0 ) /*init de database*/
  {
    printf("Error while init (CLIENT)\n");
    sleep(1);
  }

  printf("I set 1, 2 and 3\n"); /*adding new elements*/
  if( (set_value(1, "Uno", 1.0)) < 0 )
  {
    printf("Error while set (CLIENT)\n");
    sleep(10);
  }

  char aux [MAXSIZE]; /*getting a value of the server*/
  float aux2 = 40.0;
  printf("I get the values of 1\n");
  if( (get_value(1,aux, &aux2)) < 0 )
  {
    printf("Error while get (CLIENT)\n");
    sleep(1);
  }
  printf("Values of 1: value1 = %s\tvalue2 = %f\n",aux, aux2);

  if( (set_value(2, "Dos", 2.0)) < 0 )
  {
    printf("Error while set (CLIENT)\n");
    sleep(1);
  }
  if( (set_value(3, "Tres", 3.0)) < 0 )
  {
    printf("Error while set (CLIENT)\n");
    sleep(1);
  }

  aux2 = 4.0; /*modify a value*/
  printf("I modify the values of 3\n");
  if( (modify_value(3, aux, &aux2)) < 0 )
  {
    printf("Error while modify (CLIENT)\n");
    sleep(1);
  }

  printf("I get the modified values\n");
  if( (get_value(3,aux, &aux2)) < 0 ) /*get the modified value*/
  {
    printf("Error while get (CLIENT)\n");
    sleep(1);
  }
  printf("Values of 3 after modification: value1 = %s\tvalue2 = %f\n",aux, aux2);


  printf("I delete 1\n"); /*delete an element of the server*/
  if( (delete_key(1)) < 0 )
  {
    printf("Error while delete (CLIENT)\n");
    sleep(1);
  }

  int n = num_items(); /*getting the amount of elements in the server*/
  if( (n) < 0 )
  {
    printf("Error while   (CLIENT)\n");
    sleep(1);
  }
  printf("Number of elements: %i\n", n);
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
