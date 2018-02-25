#include "mensaje.h"
#include <mqueue.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>

#include "keys.c"

int main(void) {
  printf("I init the data base\n");
  init();
  printf("I set 1, 2 and 3\n");
  set_value(1, "Uno", 1.0);
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
  printf("Number of elements: %i\n", n);
  return 0;
}
