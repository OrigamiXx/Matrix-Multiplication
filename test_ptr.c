// Authors: Matt & Jerry 03/24/16

#include <stdio.h>
#include <stdlib.h>

typedef struct sum_struct{
int x;
int sum;
} sum_struct;


int sum(int x){

  int total = 0;
  int i;
  for(i = 1; i <= x; i++){

    total += i;

  }

  return total;
} 


int main(int argc, char* argv[]){


  int * A =  malloc(sizeof(int)*10);
  int * B =  malloc(sizeof(int)*10);


  int X = 0xDEADBEEF;

  int i;
  
  for (i = 0; i < 10; i++){
    A[i] = i;
    B[i] = X;
  }

  for (i = -4; i < 30; i++){
    if (i == 0 || i == 10)
      printf("--\n");
    printf("%d\n",A[i]);
  }

  free(A);
  

  printf("--------------------------------------\n");


  for (i = -4; i < 30; i++){
    if (i == 0 || i == 10)
      printf("--\n");
    printf("%d\n",A[i]);
  }

  int * C = (int *) malloc(sizeof(int)*10);

  printf("--------------------------------------\n");
  for (i = 0; i < 15; i++){
    C[i] = -i;
  }



  for (i = -4; i < 30; i++){
    if (i == 0 || i == 10)
      printf("--\n");
    printf("%d\n",A[i]);
  }


}
