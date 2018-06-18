#include <stdio.h>
#include <stdlib.h>
#include "matching.h"

int main(int argc, char * argv[]){

  bool M[] = {true, true, false, true, false, false, true, true, false};

  printf("%d\n", has_perfect_bipartite_matching(M, 3));
  
  return 0;
}
