#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "CheckUSP.h"

int main(int argc, char * argv[]){
  
  puzzle * p = create_puzzle(6,3); // correct one (8,6)

  printf("result = %d\n",CheckUSP(p));

  return 0;

}
