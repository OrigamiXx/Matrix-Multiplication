// linked list take int as data
// author Jerry

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node* next;
}Node;

struct Node* head; // global variable

void insert(int x){
  struct  Node* temp = (Node*)malloc(sizeof(struct Node));
  temp->data = x;
  temp->next = NULL;
  head = temp;
};

void print(){
  struct Node* ntemp = head;
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

}

