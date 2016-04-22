#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

int main() {
  //head = NULL;
  //int length = 10;
  LinkedList * list = LL_create();
  printf("How many numbers?\n");
  int n, i, x;
  scanf("%d",&n);
  for(i = 0; i<n; i++){
    printf("Enter the number \n");
    scanf("%d",&x);
    insert(x, list);
    print(list);
  }
  int k, j, l;
  printf("How many numbers you want insert at end?\n");
  scanf("%d",&k);
  for(j = 0; j<k;j++){
    printf("Enter the number \n");
    scanf("%d",&l);
    insert_at_end(list, l);
    print(list);
  }
  printf("How many data you want to search?\n");
  int a, b, c;
  scanf("%d",&c);
  for(i=0; i<c; i++){
    printf("What data do you want to search?\n");
    scanf("%d",&a);
    b = search(list, a);
    printf("The index is:");
    printf("%d", b);
    printf("\n");
  }
  printf("Now free the linked list.\n");
  LL_free(&list);
  printf("Freed %p\n",list);
  print(list);
  // Structure accessing
  // 1. Deference first
  //(*head).data = ..;
  //(*head).next = ..;

  // 2. Deference during
  //head->data = ..;
  //head->next = ..;

  // "head->" is short hand for "(*head)."
    
}
