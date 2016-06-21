#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "CheckUSP.h"
#include <math.h>
#include "constants.h"
#include "puzzle.h"

int main(int argc, char * argv[]){
  
  puzzle * p = create_puzzle_from_file("puzzles/usp_8_6.puz");
  print_puzzle(p);

  return 0;

}
