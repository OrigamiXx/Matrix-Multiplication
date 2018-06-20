#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"




// Returns the number of rows of the largest subpuzzle of p containing
// rows indexed > max_i that is a strong USP.
int rank(puzzle * p, int max_i){

  if (IS_USP == check(p)){
    // Should return at least s >= 1.
    return p -> s;
  }

  if (max_i < 0)
    return -1;
  
  puzzle * q = create_row_minor_puzzle(p, max_i);

  int a = rank(q, max_i - 1);
  destroy_puzzle(q);
  if (a == p -> s - 1)
    return p -> s - 1;

  int b = rank(p, max_i - 1);
  return MAX(a, b);

}

// Returns the number of rows of the largest subpuzzle of p that is a
// strong USP.
int rank(puzzle *p){
  return rank(p, p -> s - 1);
}



int main(int argc, char * argv[]){

  if (argc != 3){
    fprintf(stderr, "usage: util_submodular <s> <k>\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);

  srand48(time(NULL));
  
  puzzle * p = create_puzzle(s,k);
  randomize_puzzle(p);
  print_puzzle(p);
  printf("rank = %d\n", rank(p));
  destroy_puzzle(p);
  
  return 0;
}
