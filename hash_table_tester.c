#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "matrix.h"
#include "CheckUSP.h"
#include "hash_table.h"
#include <time.h>

void noop(void * x){
  return;
}

void print(void * x){

  printf("%d",(int)x);

}


int equals(void * x, void * y){

  return x == y;

}

int identity(void * x){

  return (int) x;

}

void main(int argc, char * argv[]) {

  hash_table * t = create_hash_table(10,identity,identity,equals);

  print_hash_table(t,print,print);

  insert_in_hash_table(t,(void *)2,(void *)3);

  print_hash_table(t,print,print);

  insert_in_hash_table(t,(void *)12,(void *)5);

  print_hash_table(t,print,print);

  insert_in_hash_table(t,(void *)22,(void *)7);

  print_hash_table(t,print,print);

  void * v;

  delete_in_hash_table(t,(void *)12,&v);
  print(v);
  printf("\n");

  print_hash_table(t,print,print);

  delete_in_hash_table(t,(void *)12,&v);

  if (search_in_hash_table(t,(void *)12,&v)) {
    print(v);
    printf("\n");
  } else {
    printf("Not found.\n");
  }

  if (search_in_hash_table(t,(void *)22,&v)) {
    print(v);
    printf("\n");
  } else {
    printf("Not found.\n");
  }

  hash_table * t2 = copy_hash_table(t,0.5);

  print_hash_table(t2,print,print);

  hash_table * t3 = copy_hash_table(t,4);

  print_hash_table(t3,print,print);

  destroy_hash_table_deep(t,noop,noop);
  destroy_hash_table_deep(t2,noop,noop);
  destroy_hash_table_deep(t3,noop,noop);

}
