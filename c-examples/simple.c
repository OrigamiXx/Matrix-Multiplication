// Authors: Matt & Jerry 03/24/16

#include <stdio.h>
#include <stdlib.h>

typedef struct sum_struct{
int x;
int sum;
} sum_struct;


int sum(int x){

  int total = 0;
  
  for(int i = 1; i <= x; i++){

    total += i;

  }

  return total;
} 


int main(int argc, char* argv[]){

  sum_struct* structs = NULL;

  if (argc == 1){
    printf("Usage: ./a.out <integer> [integer] ...\n");
    return -1;
  } else {
    
    structs = malloc(sizeof(sum_struct) * argc);  

    //printf("argc = %d\n",argc);
    for(int i = 0; i < argc-1; i++){
      structs[i].x = atoi(argv[i+1]);
      structs[i].sum = sum(structs[i].x);
      printf("structs[%d].x = %d, structs[%d].sum = %d\n",i,structs[i].x,i,structs[i].sum);
    }

    free(structs); // Prevent memory leak.

    return 0;
  }


}
