//#ifdef __LINKED_LIST_H__
//#define __LINKED_LIST_H__


typedef struct Node {
  int data;
  struct Node * next;
} Node;

typedef struct LinkedList{
  Node * head;
  int len;
} LinkedList;

LinkedList * LL_create();

void LL_free(LinkedList ** ll);

void insert_at_end(LinkedList * ll, int data);

int search(LinkedList * ll, int data);

void insert(int x, LinkedList * ll);

int print(LinkedList * ll);


//#endif
