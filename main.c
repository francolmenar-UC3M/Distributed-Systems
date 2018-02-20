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
  /*
  int key = 20;
  char value1 = "Test";
  float value2 = 280.0
  */
  fillList();//I fill the list with trivial elements
  char value1[256];//Auxiliary variables
  float value2 = 280.0;
  strcpy(value1, "Test");
  if(get_value(1, value1, &value2) < 0){
    printf("ERROR testGet_value\n");
    return-1;
  }
  else{
    printf("%s, %f\n",value1, value2);
    return 0;
  }

}

int main(int argc, char *argv[]){
  testInit();
  testContain();
  testSet_value();
  testGet_value();
  return 0;
}
