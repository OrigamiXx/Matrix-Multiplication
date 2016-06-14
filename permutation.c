//permutation in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "constants.h"

permutation  * ID_permutation(int n){
  permutation * tmp = (permutation *) malloc(sizeof(permutation));
  //tmp -> arrow[n];  // This accesses element n of tmp -> arrow.
  tmp -> arrow = malloc(sizeof(int)*n);
  int i;
  for (i = 0; i<n; i++){
    tmp->arrow[i] = i;
  }
  tmp -> size = n; 
  return tmp;
}

// apply permutation, x must be inside the domain of the permutation
int Apply_permutation(permutation * pi, int x){
  int result;
  result = pi->arrow[x];
  return result;
}
//last permutation - Takes a permutation and returns a new permutation 
//over the set of the same size which is the last permutation for that
//set.
permutation * last_permutation(int n){
  permutation * tmp = (permutation *) malloc(sizeof(permutation));
  tmp -> size = n;
  tmp -> arrow = malloc(sizeof(int)*n);
  int i;
  for (i = 0; i < tmp->size; i++){
    tmp->arrow[i] = n - (i + 1);
  }
  return tmp;
}

//next_permutation
// some problems, it won't loop back and the order is a little bit off
// Assumes that pi is not the last permutation.
permutation * next_permutation(permutation * pi){
  int j, l, k, n;
  n = pi->size-1;
  j = n -1;
  
  while (pi->arrow[j]>=pi->arrow[j+1] && j!=0){
    j--;  
  }
  l = n;
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

// Replaces and returns the inverse of a permutation.
permutation * invert_permutation(permutation * pi){

  int s = pi -> size;
  
  assert(s <= 30);

  int tmp[30];
  memcpy(tmp, pi -> arrow, sizeof(int) * s);
  

  int i;
  for (i = 0; i < s; i++){
    pi -> arrow[tmp[i]] = i;
  }

  return pi;
}



//Deallocates a permutation.
void destroy_perm(permutation * pi){
  free(pi -> arrow);
  free(pi);
}

//Tests whether two permutations are equal.
int equals(permutation * pi1, permutation * pi2){

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

//Tests whether permutation is the last permutation.
int is_last(permutation * pi){

  permutation * last = last_permutation(pi -> size);

  int ret = equals(pi,last);

  destroy_perm(last);

  return ret;
}




//compose just like fucntion f:A->B g:B->C
// int this case it's delta(pi(x))
permutation * compose(permutation * pi, permutation * delta){
  permutation * result = ID_permutation(pi -> size);
  int i;
  for (i=0; i<result->size; i++){
    result->arrow[i] = delta->arrow[pi->arrow[i]];
  }
  
  return result;
} 


int print(permutation * pi){
  int i,n;
  n = pi->size;
  for (i= 0; i<n; i++){
    printf("%d->%d",i,pi->arrow[i]);
    printf("\n");
  }
  return 0;
}


