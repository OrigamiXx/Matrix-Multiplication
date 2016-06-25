#include <stdio.h>
#include <stdlib.h>
#include "set.h"
#include "constants.h"

int main(int argc, char * argv[]){
  set a = create_empty_set();
  print_set(a);
  set b = create_one_element_set(5);
  if (is_membership(5,b)){
    printf("yes\n");
  }
  printf("%d\n", b & 1<<5);
  print_set(b);
  set c = create_one_element_set(3);
  set d = set_union(b,c);
  print_set(d);



  return 0;
}
