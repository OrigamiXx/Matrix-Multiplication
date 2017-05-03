// linked list take int as data
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

LinkedList * LL_create(){
  LinkedList * tmp = (LinkedList *) malloc(sizeof(LinkedList));
  tmp -> head = NULL;
  tmp -> len = 0;
  return tmp;
}

// Free all nodes in list and LL struct.
void LL_free(LinkedList ** ll){
  Node * current = (*ll)->head; // equivalently, (**ll).head      
  Node * next; 
  while (current != NULL){
    next = current->next;
    free(current);
    current = next;  
  }
  
  (*ll)->head = NULL;
  (*ll)->len = 0;
  free(*ll);
  *ll = NULL;
}

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

// Returns the index of data given, -1 if not found.
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


void insert(int x, LinkedList * ll){
  Node * temp = (Node *)malloc(sizeof(Node));
  temp->data = x;
  temp->next = ll->head;
  ll->head = temp;
  ll->len++;
};

int print(LinkedList * ll){

  if (ll != NULL){

    Node * ntemp = ll->head;
    printf("List is: ");

    if (ntemp == NULL)
      printf("nothing is in the list");

    while(ntemp != NULL){
      printf(" %d",ntemp->data);
      ntemp= ntemp->next;
    }

    printf("\n");

    return 0;
  }
  
  return -1;

}


