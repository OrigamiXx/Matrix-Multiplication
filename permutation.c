/*
  This module contains functions and datatypes for representing
  permutations.  There are functions for creating, copying,
  destroying, applying, inverting, displaying and comparing
  permutations.

  Authors: Jerry, Anthony, Matt.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "constants.h"
#include <math.h>

// Macro for swapping contents of variables with _same_ type.
#define SWAP(x, y)  do { (x) = (x) ^ (y);  (y) = (x) ^ (y);  (x) = (x) ^ y; } while (0)

/*=================================================================

   Permutation Creation 

   Remember to deallocate!

=================================================================*/

/*
  Allocates, creates, and returns an permutation of size n.  The array
  arrow has invalid contents.  Private.
*/
perm * __create_empty_perm(int n){

  perm * pi = (perm *) malloc(sizeof(perm));
  int * arrow = (int *) malloc(sizeof(int) * n);
  assert(pi != NULL && arrow != NULL);  // Fail hard if out of memory.
  
  pi -> n = n;
  pi -> arrow = arrow;

  return pi;  
}

/*
  Allocates, creates, and returns an identity permutation of size
  n. An identity permutation is a permutation that maps every element
  of the base set U to itself.  In cycle notation this is
  (0)(1)(2)...(n-1).
*/
perm  * create_perm_identity(int n){

  perm * pi = __create_empty_perm(n);
  
  for (int i = 0 ; i < n ; i++)
    pi->arrow[i] = i;
  
  return pi;
}

/*
  Allocates, creates, and returns the last permutation of size n.
  This permutation has cycle representation (n-1 n-2 n-3 ... 1 0).
*/
perm * create_last_perm(int n){

  perm * pi = __create_empty_perm(n);

  for (int i = 0 ; i < n ; i++)
    pi -> arrow[i] = n - (i + 1);
  
  return pi;
}


/*
  Allocates, creates, and returns a copy of the given permutation pi.
*/
perm * copy_perm(perm * pi){
  
  int n = pi -> n;
  perm * pi2 = __create_empty_perm(n);

  memcpy(pi2 -> arrow, pi -> arrow, sizeof(int) * n);

  return pi2;
}

/*
  Deallocates given permutation.
*/
void destroy_perm(perm * pi){

  free(pi -> arrow);
  free(pi);

}

/*=================================================================

   Permutation Operations

=================================================================*/

/* 
   Allocates a new permutation which is the result of composing the
   two given permutations, in particular it produces the permutation
   rho which maps each x in U to delta(pi(x)).
*/
perm * compose_perm(perm * pi, perm * delta){

  int n = pi -> n;
  perm * rho = __create_empty_perm(n);

  for (int i = 0 ; i < n ; i++)
    rho -> arrow[i] = delta -> arrow[pi -> arrow[i]];

  return rho;
} 

/* 
   Mutates the given permutation into the next permutation in
   lexicographic order.  Implements Knuth's Algorithm L.  Runtime
   O(n).
*/
perm * next_perm(perm * pi){
  int i, j, k, n;
  int * arrow = pi -> arrow;
  
  n = pi -> n - 1;
  j = n - 1;

  while (arrow[j] >= arrow[j + 1] && j != 0)
    j--;  

  i = n;
    
  while (arrow[j] >= arrow[i])
    i--;

  SWAP(arrow[i], arrow[j]);

  k = j + 1;
  i = n;
  while (k < i){
    SWAP(arrow[k], arrow[i]);
    k++;
    i--;
  }

  return pi;
}

/* 
   Mutates the given permutation into its inverse.
*/
perm * invert_perm(perm * pi){

  int n = pi -> n;
  int * arrow = pi -> arrow;

  for (int i = 0; i < n; i++){

    if (arrow[i] < 0)
      arrow[i] = -arrow[i] - 1;
    else {
      int start = i;
      int prev = i;
      int curr = arrow[i];
      while (curr != start){
	int next = arrow[curr]; 
	arrow[curr] = -prev - 1;
	prev = curr;
	curr = next;
      }
      arrow[start] = prev;
    }

  }

  return pi;
}


/*=================================================================

   Permutation Comparisons

=================================================================*/

/*
  Returns true iff the given permutation pi is the identity perm.
*/
int is_identity_perm(perm * pi){
  
  int n = pi -> n;
  int * arrow = pi -> arrow;
  
  for (int i = 0; i < n; i++)
    if (arrow[i] != i)
      return false;

  return true;
}

/*
  Returns true iff the given permutations are identical.
*/
int is_equals_perm(perm * pi1, perm * pi2){

  if (pi1 == pi2)
    return true;

  if (pi1 == NULL || pi2 == NULL || pi1 -> n != pi2 -> n)
    return false;

  if (memcmp(pi1 -> arrow, pi2 -> arrow, pi1 -> n * sizeof(int)) == 0)
    return true;

  return false;
}

/*
  Returns true iff the given permutation is the lexicographically last
  permutation.
*/
int is_last_perm(perm * pi){

  int n_minus = pi -> n - 1;
  int * arrow = pi -> arrow;
  int i = 0;

  while (i <= n_minus && (i + arrow[i]) == n_minus)
    i++;

  return i > n_minus;
}



/*=================================================================

   Permutation Display

=================================================================*/

/*
  Displays the given permutation as a function table to the console.
*/
void print_perm_tabular(perm * pi){

  int n = pi -> n;
  int * arrow = pi -> arrow;
  
  for (int i = 0 ; i < n ; i++)
    printf("%d -> %d\n", i, arrow[i]);
}

/*
  Displays the given permutation in cycle notation to the console.
*/
void print_perm_cycle(perm * pi){

  int n = pi -> n;
  int * arrow = pi -> arrow;
  
  for (int i = 0 ; i < n ; i++)

    if (arrow[i] < 0)
      
      arrow[i] = -arrow[i] - 1;

    else {
	       
      int start = i;
      int curr = start;
      int next = apply_perm(pi, curr);

      if (curr == next)
	printf("(%d)", curr);
      else {

	printf("(");
	
	while (next != start) {
	  
	  printf("%d ", curr);
	  curr = next;
	  next = apply_perm(pi, curr);
	  arrow[curr] = -arrow[curr] - 1;
	}
	
	printf("%d)", curr);
	
      }
	
    }

  printf("\n");

}


