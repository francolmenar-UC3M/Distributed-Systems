#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/*https://www.tutorialspoint.com/data_structures_algorithms/doubly_linked_list_program_in_c.htm*/

/*Each node is a triplet*/
static struct node {
   int key;//The key of the triplet
   char value1[256];//First value of the triplet
   float value2;//Second value of the triplet

   struct node *next;
   struct node *prev;
};

//this link always point to first Link
static struct node *head = NULL;

//this link always point to last Link
static struct node *last = NULL;

static struct node *current = NULL;

//is list empty
static bool isEmpty() {
   return head == NULL;
}

static int length() {
   int length = 0;
   struct node *current;

   for(current = head; current != NULL; current = current->next){
      length++;
   }
   return length;
}

//display the list in from first to last
static void displayForward() {

   //start from the beginning
   struct node *ptr = head;

   //navigate till the end of the list
   printf("\n[ ");

   while(ptr != NULL) {
      printf("(%i,%s, %f) ",ptr->key,ptr->value1,ptr->value2);
      ptr = ptr->next;
   }

   printf(" ]");
}

//display the list from last to first
static void displayBackward() {

   //start from the last
   struct node *ptr = last;

   //navigate till the start of the list
   printf("\n[ ");

   while(ptr != NULL) {

      //print data
      printf("(%i,%s, %f) ",ptr->key,ptr->value1,ptr->value2);

      //move to next item
      ptr = ptr ->prev;

   }


}

//insert link at the first location
static void insertFirst(int key, char* value1, float value2) {

   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   link->key = key;
   strcpy(link->value1,value1);
   link->value2 = value2;

   if(isEmpty()) {
      //make it the last link
      last = link;
   } else {
      //update first prev link
      head->prev = link;
   }

   //point it to old first link
   link->next = head;

   //point first to new first link
   head = link;
}

//insert link at the last location
static void insertLast(int key,  char* value1, float value2) {

   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   link->key = key;
   strcpy(link->value1,value1);
   link->value2 = value2;

   if(isEmpty()) {
      //make it the last link
      last = link;
   } else {
      //make link a new last link
      last->next = link;

      //mark old last node as prev of new link
      link->prev = last;
   }

   //point last to new last node
   last = link;
}

//delete first item
static struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = head;

   //if only one link
   if(head->next == NULL){
      last = NULL;
   } else {
      head->next->prev = NULL;
   }

   head = head->next;
   //return the deleted link
   return tempLink;
}

//delete link at the last location
static struct node* deleteLast() {
   //save reference to last link
   struct node *tempLink = last;

   //if only one link
   if(head->next == NULL) {
      head = NULL;
   } else {
      last->prev->next = NULL;
   }

   last = last->prev;

   //return the deleted link
   return tempLink;
}

//delete a link with given key
static struct node* delete(int key) {

   //start from the first link
   struct node* current = head;
   struct node* previous = NULL;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {
      //if it is last node

      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;

         //move to next link
         current = current->next;
      }
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      current->prev->next = current->next;
   }

   if(current == last) {
      //change last to point to prev link
      last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
   return current;
}

/*It checks if an element exists in the list*/
static bool contain(int key){
  //start from the first link
  struct node *current = head;

  //if list is empty
  if(head == NULL) {
     return false;
  }

  //navigate through list
  while(current->key != key) {
     //if it is last node
     if(current->next == NULL) {
        return false;
     } else {
        //move to next link
        current = current->next;
     }
  }
  return true;
}

/*It copies the values of a given triplet into the memory addreses*/
static bool obtainElement(int key, char *value1, float *value2){
  //start from the first link
  struct node *current = head;

  //if list is empty
  if(head == NULL) {
     return false;
  }

  //navigate through list
  while(current->key != key) {
     //if it is last node
     if(current->next == NULL) {
        return false;
     } else {
        //move to next link
        current = current->next;
     }
  }
  strcpy(value1, current -> value1);
  *value2 = current -> value2;
  return true;
}

/*It changes the values of a given triplet*/
static bool changeValues(int key, char *value1, float *value2){
  //start from the first link
  struct node *current = head;

  //if list is empty
  if(head == NULL) {
     return false;
  }

  //navigate through list
  while(current->key != key) {
     //if it is last node
     if(current->next == NULL) {
        return false;
     } else {
        //move to next link
        current = current->next;
     }
  }
  strcpy(current -> value1, value1);
  current -> value2 = *value2;
  return true;
}


static bool insertAfter(int key, int newKey, char* value1, float value2) {
   //start from the first link
   struct node *current = head;

   //if list is empty
   if(head == NULL) {
      return false;
   }

   //navigate through list
   while(current->key != key) {

      //if it is last node
      if(current->next == NULL) {
         return false;
      } else {
         //move to next link
         current = current->next;
      }
   }

   //create a link
   struct node *newLink = (struct node*) malloc(sizeof(struct node));
   newLink->key = newKey;
   newLink->value2 = value2;

   if(current == last) {
      newLink->next = NULL;
      last = newLink;
   } else {
      newLink->next = current->next;
      current->next->prev = newLink;
   }

   newLink->prev = current;
   current->next = newLink;
   return true;
 }
