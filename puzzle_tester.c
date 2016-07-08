#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include "puzzle.h"



int main(int argc, char * argv[]){
  string raw_input = "puzzles/test.puz";
  const char * input = raw_input.c_str();
  puzzle * p = create_puzzle_from_file(input);
  //print_puzzle(p);
  write_puzzle(p, 0);
  puzzle * p1 = create_puzzle_from_puzzle(p, 1);
  print_puzzle(p1);

  return 0;

}
