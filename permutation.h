#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

//In this structure, perm is organzied by a int array. Each index is 
//assigned to a different number so the whole array represents a 
//permutation. So an array 1 2 3 means T[1] -> 1, T[2] -> 2 and 
//T[3] -> 3. 
typedef struct perm {  // Rename: perm
  int size;
  int * arrow;
} perm;

//Create an identity permutation with size n. Identity permutation 
//is a permutation that the index is equals to its value. So its 
//T[1] -> 1, T[2] -> 2.. T[n] -> n
perm * create_perm_identity(int n);  // Rename: create_perm_identity

// Returns a newly allocated copy of pi.
perm * copy_perm(perm * pi); // Rename: copy_perm

//Give the input index and return the value assoicate with the index. 
//Basically return pi[x]
int apply_perm(perm * pi, int x); // Rename: apply_perm

// Returns true iff pi is the identity perm. // Rename: is_identity_perm
int is_identity_perm(perm * pi);

//Move the perm into the next combination. Say the next perm for T[0] ->0,
//T[1] -> 1, T[2] -> 2 is T[0] -> 0, T[2] -> 1. T[1] -> 2
perm * next_perm(perm * pi);  // Rename: next_perm

// Replaces and returns the inverse of a perm.
perm * inverse_perm(perm * pi);  // Rename: inverse_perm

//Combine the two perms together. 
perm * compose_perm(perm * pi, perm * delta); // Rename: compose_perm
	
int print_perm(perm * pi);  // Rename: print_perm

//Create the last perm. Which is T[0] -> n, T[1] -> n-1..T[n] -> 0
perm * create_last_perm(int n); // Rename: create_perm_last

//Free perm
void destroy_perm(perm * pi);  // Ok

//Check if two perms are equals
int is_equals_perm(perm * pi1, perm * pi2); // Rename: is_equal_perm

//Check fi the perm is last perm
int is_last_perm(perm * pi); 

//Print the perm in compact form. So all the value that are point to other index
//beside itself will be paired up. Say if the perm is T[0] -> 1. T[1] -> 2, T[2] -> 0
//, T[3] -> 3, the perm will be represent in the form of (0 1 2) 3
void print_compact_perm(perm * pi);

#endif
