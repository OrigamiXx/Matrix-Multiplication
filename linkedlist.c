// linked list take int as data
// author Jerry

#include <stdio.h>
#include <stdlib.h>



typedef struct Node {
  int data;
  struct Node * next;
} Node;

typedef struct LinkedList{
  Node * head;
  int len;
} LinkedList;

LinkedList * LL_create(){

  LinkedList * tmp = (LinkedList *) malloc(sizeof(LinkedList));
  tmp -> head = NULL;
  tmp -> len = 0;

  return tmp;

}

// TODO: void LL_free(LinkedList *) - Free all nodes in list and header struct.

// TODO: Remove
Node * head = NULL; // global variable

// TODO: void insert_at_end(LinkedList *,int)

// TODO: int search(LinkedList *,int) - Returns the index of data given, -1 if not found.

// TODO: Modify to use LinkedList struct.
void insert(int x){
  Node * temp = (Node *)malloc(sizeof(Node));
  temp->data = x;
  temp->next = head;
  head = temp;
};

// TODO: Modify to use LinkedList struct.
void print(){
  Node * ntemp = head;
  printf("List is: ");
  while(ntemp != NULL){
    printf(" %d",ntemp->data);
    ntemp= ntemp->next;
  }
  printf("\n");
}

int main() {
  head = NULL;
  //int length = 10;
  printf("How many numbers?\n");
  int n, i, x;
  scanf("%d",&n);
  for(i = 0; i<n; i++){
    printf("Enter the number \n");
    scanf("%d",&x);
    insert(x);
    print();
  }

  // Structure accessing
  // 1. Deference first
  //(*head).data = ..;
  //(*head).next = ..;

  // 2. Deference during
  //head->data = ..;
  //head->next = ..;

  // "head->" is short hand for "(*head)."
    
}

