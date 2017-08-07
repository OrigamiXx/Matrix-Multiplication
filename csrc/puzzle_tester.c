// Simple tester for the puzzle module.

#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include "constants.h"
#include "puzzle.h"


int main(int argc, char * argv[]){
  
  const char * input_file = "puzzles/test.puz";
  
  puzzle * p = create_puzzle_from_file(input_file);
  print_puzzle(p);

  puzzle * p1 = create_puzzle_from_puzzle(p, 1);
  print_puzzle(p1);

  return 0;

}
