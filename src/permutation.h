#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

/*
  This module contains functions and datatypes for representing
  permutations.  There are functions for creating, copying,
  destroying, applying, inverting, displaying and comparing
  permutations.

  Authors: Jerry, Anthony, Matt.
*/


/*
  Each permutation over the base set U = {0, 1, 2, ..., n-1} is
  represented via the a structure perm, which stores (i) it's domain
  size, n, and (ii) a pointer to an integer array arrow of length n
  storing distinct values from U.  The permutation maps i to arrow[i].

  For example, the permutation (0 2 1) is represented by an arrow =
  [2, 0, 1].
 */
typedef struct perm {  
  int n;
  int * arrow;
} perm;


/*=================================================================

   Permutation Creation and Destruction

   Remember to deallocate!

=================================================================*/

/*
  Allocates, creates, and returns an identity permutation of size
  n. An identity permutation is a permutation that maps every element
  of the base set U to itself.  In cycle notation this is
  (0)(1)(2)...(n-1).
*/
perm * create_perm_identity(int n); 

/*
  Allocates, creates, and returns the last permutation of size n.
  This permutation has cycle representation (n-1 n-2 n-3 ... 1 0).
*/
perm * create_last_perm(int n);

/*
  Allocates, creates, and returns a copy of the given permutation pi.
*/
perm * copy_perm(perm * pi); 

/*
  Deallocates given permutation.
*/
void destroy_perm(perm * pi); 


/*=================================================================

   Permutation Operations

=================================================================*/

/* 
   Takes a permutation pi over the base set U = {0, 1, ..., n-1} and
   an element x of U and returns the result of applying pi to x.
   Inline.  Runtime O(1).
*/
inline int apply_perm(perm * pi, int x) {
  return pi -> arrow[x];
}
  
/* 
   Allocates a new permutation which is the result of composing the
   two given permutations, in particular it produces the permutation
   rho which maps each x in U to delta(pi(x)).  Runtime O(n).
*/
perm * compose_perm(perm * pi, perm * delta); 

/* 
   Mutates the given permutation into the next permutation in
   lexicographic order.  Implements Knuth's Algorithm L.  Runtime
   O(n).
*/
perm * next_perm(perm * pi);

/* 
   Mutates the given permutation into its inverse.  Runtime O(n).
*/
perm * invert_perm(perm * pi);

/*=================================================================

   Permutation Comparisons

=================================================================*/

/*
  Returns true iff the given permutation pi is the identity.
*/
int is_identity_perm(perm * pi);

/*
  Returns true iff the given permutations are identical.
*/
int is_equals_perm(perm * pi1, perm * pi2); 

/*
  Returns true iff the given permutation is the lexicographically last
  permutation.
*/
int is_last_perm(perm * pi); 


/*=================================================================

   Permutation Display

=================================================================*/

/*
  Displays the given permutation as a function table to the console.
*/
void print_perm_tabular(perm * pi);

/*
  Displays the given permutation in cycle notation to the console.
*/
void print_perm_cycle(perm * pi);






#endif
