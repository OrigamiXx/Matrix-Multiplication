#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include "puzzle.h"

int main(int argc, char * argv[]){
  
  puzzle * p = create_puzzle_from_file("puzzles/test.puz");
  print_puzzle(p);
  write_puzzle(p, 0);

  return 0;

}
