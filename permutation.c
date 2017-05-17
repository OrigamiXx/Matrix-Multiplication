//permutation in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "constants.h"
#include <math.h>

//create an array A with size of n that A[i] = i
perm  * create_perm_identity(int n){
  perm * tmp = (perm *) malloc(sizeof(perm));
  assert(tmp != NULL);
  //tmp -> arrow[n];  // This accesses element n of tmp -> arrow.
  tmp -> arrow = (int*)malloc(sizeof(int)*n);
  int i;
  for (i = 0; i<n; i++){
    tmp->arrow[i] = i;
  }
  tmp -> size = n; 
  return tmp;
}

// Returns a newly allocated copy of pi.
perm * copy_perm(perm * pi){
  
  int size = pi -> size;
  perm * pi2 = create_perm_identity(size);

  int i;
  for (i = 0; i < size; i++){
    pi2 -> arrow[i] = pi -> arrow[i];
  }

  return pi2;

}

// apply perm, x must be inside the domain of the perm
//apply-perm takes in x and return the index x of the array in pi

//Return the x element in pi
int apply_perm(perm * pi, int x){
  int result;
  result = pi->arrow[x];
  return result;
}

//last perm - Takes a perm and make goes from n-1 to 0
perm * create_last_perm(int n){
  perm * tmp = (perm *) malloc(sizeof(perm));
  tmp -> size = n;
  tmp -> arrow = (int*)malloc(sizeof(int)*n);
  int i;
  for (i = 0; i < tmp->size; i++){
    tmp->arrow[i] = n - (i + 1);
  }
  return tmp;
}

//next_perm
// some problems, it won't loop back and the order is a little bit off
// Assumes that pi is not the last perm.
//??
perm * next_perm(perm * pi){
  int j, l, k, n;
  n = pi->size-1;
  j = n -1;
  
    //j: index of first element that is smaller than it's next element
  while (pi->arrow[j]>=pi->arrow[j+1] && j!=0){
    j--;  
  }
  l = n;
    
    //l: Should usually be the next element of j
    
    //May break for 1432
  while (pi->arrow[j]>=pi->arrow[l]){
    l--;
  }
  int temp;
  temp = pi->arrow[j];
  pi->arrow[j] = pi->arrow[l];
  pi->arrow[l] = temp;
  k = j + 1;
  l = n;
  while (k < l){
    temp = pi->arrow[k];
    pi->arrow[k] = pi->arrow[l];
    pi->arrow[l] = temp;
    k = k+1;
    l = l-1;
  }
  return pi;
}

// Replaces and returns the inverse of a perm.
perm * inverse_perm(perm * pi){

  int s = pi -> size;

  int i;
  for (i = 0; i < s; i++){

    if (pi -> arrow[i] < 0)
      pi -> arrow[i] = - pi -> arrow[i] - 1;
    else {
      int start = i;
      int prev = i;
      int curr = pi -> arrow[i];
      while (curr != start){
	int next = pi -> arrow[curr]; 
	pi -> arrow[curr] = -prev - 1;
	prev = curr;
	curr = next;
      }
      pi -> arrow[start] = prev;
    }

  }

  return pi;
}

// Returns true iff pi is the identity perm.
int is_identity_perm(perm * pi){
  
  int s = pi -> size;
  
  int i;
  for (i = 0; i < s; i++){
    if (pi -> arrow[i] != i)
      return false;
  }

  return true;

}


//Deallocates a perm.
void destroy_perm(perm * pi){
  free(pi -> arrow);
  free(pi);
}

//Tests whether two perms are equal.
int is_equals_perm(perm * pi1, perm * pi2){

  if (pi1 == NULL && pi2 == NULL)
    return true;
  if (pi1 == NULL || pi2 == NULL)
    return false;

  if (pi1 -> size != pi2 -> size)
    return false;
  
  int i;

  for (i = 0; i < pi1 -> size; i++)
    if (pi1 -> arrow[i] != pi2 -> arrow[i])
      return false;

  return true;
}

//Tests whether perm is the last perm.
int is_last_perm(perm * pi){

  perm * last = create_last_perm(pi -> size);

  int ret = is_equals_perm(pi,last);

  destroy_perm(last);

  return ret;
}




//compose just like fucntion f:A->B g:B->C
// int this case it's delta(pi(x))
perm * compose_perm(perm * pi, perm * delta){
  perm * result = create_perm_identity(pi -> size);
  int i;
  for (i=0; i<result->size; i++){
    result->arrow[i] = delta->arrow[pi->arrow[i]];
  }
  
  return result;
} 


//Print out the elements in pi in a form (i, arrow[i], arrow[arrow[i]]..).
int print_perm(perm * pi){
  int i,n;
  n = pi->size;
  for (i= 0; i<n; i++){
    printf("%d->%d",i,pi->arrow[i]);
    printf("\n");
  }
  return 0;
}

void print_compact_perm(perm * pi){

  int visited[30];

  assert(pi -> size <= 30);

  int i, j;

  for (i = 0; i < pi -> size; i++){
    visited[i] = false;
  }

  for (i = 0; i < pi -> size; i++){

    if (!visited[i]){
      j = i;
      printf("(");
      while (!visited[j]){
	visited[j] = true;
	printf("%d",j);
	j = apply_perm(pi,j);
	if (!visited[j])
	  printf(" ");
      }
      printf(")");
    }

  }


}


