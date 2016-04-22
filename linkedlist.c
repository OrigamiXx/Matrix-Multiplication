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
void LL_free(LinkedList * ll){
  Node * current = ll->head;
  Node * next; 
  while (current != NULL){
    next = current->next;
    free(current);
    current = next;  
  }
  
  ll->head = NULL;
  ll->len = 0;
}

// TODO: void insert_at_end(LinkedList *,int)
void insert_at_end(LinkedList * ll, int data){
  Node * new_node;
  Node * temp;
  new_node = (Node *)malloc(sizeof(Node));
  new_node->data = data;
  new_node->next = NULL;
  temp = ll->head;
  if (temp == NULL){
    temp = new_node;
  }else {
    while(temp->next != NULL){
      temp = temp->next;
    }
    temp->next = new_node;
  }

}

// TODO: int search(LinkedList *,int) - Returns the index of data given, -1 if not found.
int search(LinkedList * ll, int data){
  Node * current = ll->head;
  int index = 0;
  while(current != NULL){
    if (current->data == data){
      return index;
    }else{
      current = current->next;
      index++;
    }
  }
  return -1;
}


// TODO: Modify to use LinkedList struct.
void insert(int x, LinkedList * ll){
  Node * temp = (Node *)malloc(sizeof(Node));
  temp->data = x;
  temp->next = ll->head;
  ll->head = temp;
  ll->len++;
};

// TODO: Modify to use LinkedList struct.
void print(LinkedList * ll){
  Node * ntemp = ll->head;
  printf("List is: ");
  if (ntemp == NULL)
    printf("nothing is in the list");
  while(ntemp != NULL){
    printf(" %d",ntemp->data);
    ntemp= ntemp->next;
  }
  printf("\n");
}

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
  LL_free(list);
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

