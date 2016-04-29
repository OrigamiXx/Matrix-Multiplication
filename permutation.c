//permutation in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

permutation  * ID_permutation(int n){
  permutation * tmp = (permutation *) malloc(sizeof(permutation));
  tmp -> arrow[n];
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


//next_permutation??????


//compose just like fucntion f:A->B g:B->C
// int this case it's delta(pi(x))
permutation * compose(permutation * pi, permutation * delta){
  permutation * result = (permutation *)malloc(sizeof(permutation));
  result -> size = pi->size;
  result -> arrow[result->size];
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
  printf("What number(must be less than the size) do you want to apply in the permutation?\n");
  int x;
  scanf("%d", &x);
  printf("The output is:");
  printf("%d",Apply_permutation(f,x));
  printf("\n");
  printf("The compose of g o f is:\n");
  print(compose(f,g));

  return 0;
}
