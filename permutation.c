//permutation in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

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
//last permutation
permutation * last_permutation(permutation * pi){
  permutation * tmp = (permutation *) malloc(sizeof(permutation));
  tmp -> arrow = malloc(sizeof(int)*pi->size);
  int i;
  for (i = 0; i< pi->size; i++){
    tmp->arrow[i] = pi->arrow[pi->size-1-i] ;
  }
  tmp -> size = pi->size;
  return tmp;
}

//next_permutation
// some problems, it won't loop back and the order is a little bit off
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




