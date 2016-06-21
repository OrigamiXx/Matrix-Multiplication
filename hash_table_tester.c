#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "matrix.h"
#include "usp.h"
#include "hash_table.h"
#include <time.h>


void main(int argc, char * argv[]) {

  hash_table * t = create_hash_table(10,identity_helper,identity_helper,equals_helper);

  print_hash_table(t,print_helper,print_helper);

  insert_in_hash_table(t,(hash_val)2,(hash_val)3);

  print_hash_table(t,print_helper,print_helper);

  insert_in_hash_table(t,(hash_val)12,(hash_val)5);

  print_hash_table(t,print_helper,print_helper);

  insert_in_hash_table(t,(hash_val)22,(hash_val)7);

  print_hash_table(t,print_helper,print_helper);

  hash_val * v;

  delete_in_hash_table(t,(hash_val)12,&v);
  print_helper(*v);
  printf("\n");

  print_hash_table(t,print_helper,print_helper);

  delete_in_hash_table(t,(hash_val)12,&v);

  if (search_in_hash_table(t,(hash_val)12,&v)) {
    print_helper(*v);
    printf("\n");
  } else {
    printf("Not found.\n");
  }

  if (search_in_hash_table(t,(hash_val)22,&v)) {
    print_helper(*v);
    printf("\n");
  } else {
    printf("Not found.\n");
  }

  hash_table * t2 = copy_hash_table(t,0.5);

  print_hash_table(t2,print_helper,print_helper);

  hash_table * t3 = copy_hash_table(t,4);

  print_hash_table(t3,print_helper,print_helper);

  destroy_hash_table_deep(t,noop_helper,noop_helper);
  destroy_hash_table_deep(t2,noop_helper,noop_helper);
  destroy_hash_table_deep(t3,noop_helper,noop_helper);

}
