#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include "server.h"

  /*It allows the initialization of the system
  All the triplets stored in the system are destroyed
  It returns 0 in success and -1 on error*/
  int init(){return -1;}

  /*Inserts a triplet
  It returns 0 in success and -1 on error
  Trying to insert a key that already exists is considered an error*/
  int set_value(int key, char *value1, float value2){printf("praa\n");return -1;}

  /*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
  int get_value(int key, char *value1, float value2){return -1;}

  /*Modifies the values associated with a key
  It returns 0 in success and -1 on error
  Not existing an element with that key is considered an error*/
  int modify_value(int key, char *value1, float value2){return -1;}

  /*It deletes an element
  Not existing an element with that key is considered an error*/
  int delete_key(int key){return -1;}

  /*It stores the number of elements stored
  It returns 0 in success and -1 on error*/
  int num_items(){return -1;}
