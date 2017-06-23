//set in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "set.h"
#include "constants.h"
#include <assert.h>



// create a empty set
set create_empty_set(){
  set tmp = 0;
  return tmp;
}

// create a set that has only one element input
// the number of the element (can be only from 0-31)
set create_one_element_set(int element){
  set tmp = 1 << element;
  return tmp;
}


//take two sets and returns the union of them
set set_union(set a, set b){
  set tmp = a | b;
  return tmp;
}

//take two sets and returns their intersections
set set_intersect(set a, set b){
  set tmp = a & b;
  return tmp;
}


//check if the set it's empty return ture if it is
int is_empty_set(set a){
  if(a == 0){
    return true;
  }
  else{
    return false;
  }
}


//check whether the integer element is in the given set
int is_membership(int element, set a){
  set tmp = create_one_element_set(element);
  int result = a & tmp;
  if(result != 0){
    return true;
  }else{
    return false;
  }
}

//find the complement of the set and the universe is the length of the puzzle's row
set set_complement(int universe, set a){
  int negation = ~a;
  set tmp = universe & negation;
  return tmp;
}

//print out a set
void print_set(set a){
  int i, range = 32;
  int last_element;
  for (i = range-1; i >= 0; i--){
    if (is_membership(i,a)){
      last_element = i;
      break;
    }
  }
  //printf("%d\n", last_element);
  printf("{");
    for (i = 0; i<range; i++){
    if(is_membership(i,a) && i != last_element){
      printf("%d,", i);
    }
    if(is_membership(i,a) && i == last_element){
      printf("%d", i);
    }
  }
  printf("}\n");
}
