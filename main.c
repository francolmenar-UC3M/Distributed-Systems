#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include "server.c"
#define PRINTEA 0 //If it is 1 the messages are printed


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
    printf("It contains the correct values %f and %s (CORRECT)\n", value1, value2);
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

int main(int argc, char *argv[]){
  testAll();
  return 0;
}
