// Simple tester for the puzzle module.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "checker.h"
#include "constants.h"
#include "puzzle.h"

int main(int argc, char * argv[]){

  srand48(time(NULL));
  //const char * input_file = "puzzles/test.puz";
  int s = 10;
  for (unsigned int k = 2; k <= MAX_K; k++){
    puzzle * p = create_puzzle(s, k);
    printf("s = %d, k = %d, max_row = %lu\n", p -> s, p -> k, p -> max_row);
    randomize_puzzle(p);
    for(unsigned int r = 0; r < p -> s; r++){
      for(unsigned int c = 0; c < p -> k; c++){
	fprintf(stdout, "%d", get_entry(p, r, c));
      }
      fprintf(stdout, "\n");
    }

    print_puzzle(p);
  }


  /*
  puzzle * p = create_puzzle_from_file(input_file);
  print_puzzle(p);

  puzzle * p1 = create_puzzle_from_puzzle(p, 1);
  print_puzzle(p1);
  */
  return 0;

}
