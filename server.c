#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include "server.h"
#include "doubleLinkedList.c"

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
    /*
    printf("praa\n");
       insertFirst(1,"Uno",10.0);
       insertFirst(2,"Dos",20.0);
       insertFirst(3,"Tres",30.0);
       insertFirst(4,"Cuatro",1.0);
       insertFirst(5,"Cinco",40.0);
       insertFirst(6,"Seis",56.0);

       printf("\nList (First to Last): ");
       displayForward();

       printf("\n");
       printf("\nList (Last to first): ");
       displayBackward();

       printf("\nList , after deleting first record: ");
       deleteFirst();
       displayForward();

       printf("\nList , after deleting last record: ");
       deleteLast();
       displayForward();

       printf("\nList , insert after key(4) : ");
       insertAfter(4,7,"Nuevo", 13.0);
       displayForward();

       printf("\nList  , after delete key(4) : ");
       delete(4);
       displayForward();
       */
    return 0;
  }

  /*It obtains the values associated with the key
  The values are returned in value1 value2
  It returns 0 in success and -1 on error*/
  static int get_value(int key, char *value1, float *value2){
    if(!contain(key)) return -1;//The triplet does not exist
    if(obtainElement(key, value1, value2) < 0){//I get the values of the triplets
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
