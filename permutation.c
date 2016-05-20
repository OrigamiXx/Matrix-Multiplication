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



int main(){
  printf("What's the size of the permutation?\n");
  int n;
  scanf("%d", &n);
  permutation * f = ID_permutation(n);
  permutation * g = ID_permutation(n);
  printf("print out the identity permutation:\n");
  printf("f is:\n");
  print(f);
  printf("g is:\n");
  print(g);
  permutation * h = last_permutation(f);
  print(h);
  printf("What number(must be less than the size) do you want to apply in the permutation?\n");
  int x;
  scanf("%d", &x);
  printf("The output is:");
  printf("%d",Apply_permutation(f,x));
  printf("\n");
  printf("The compose of g o f is:\n");
  print(compose(f,g));
  printf("\n");
  printf("Next permutation of f:\n");
  permutation * f1 = next_permutation(f);
  print(f1);
  printf("Next permutation of f1:\n");
  permutation * f2 = next_permutation(f1);
  print(f2);
  
  return 0;
}
