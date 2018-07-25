// Simple tester for the puzzle module.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "checker.h"
#include "constants.h"
#include "puzzle.h"
#include "canonization.h"

int main(int argc, char * argv[]){

  srand48(time(NULL));
  /*
  const char * input_file = "puzzles/test.puz";
  puzzle * p = create_puzzle_from_file(input_file);
  printf("s = %d, k = %d, max_row = %lu\n", p -> s, p -> k, p -> max_row);
  print_puzzle(p);
  printf("----------------\n");
  randomize_puzzle(p);
  print_puzzle(p);
  printf("----------------\n");
  canonize_puzzle(p);
  */
  /*
  int num = 100000;
  int num_unique = 0;
  int num_unique_usp = 0;
  for (int i = 0; i < num; i++){
    randomize_puzzle(p);
    if (!have_seen_isomorph(p)){
      num_unique++;
      if (IS_USP == check(p)){
	num_unique_usp++;
	canonize_puzzle(p);
	assert(IS_USP == check(p));
	print_puzzle(p);
	printf("----------------\n");
      }
    }
  }
  printf("Unique %d / %d / %d\n", num_unique_usp, num_unique, num);
  */

  puzzle * p1 = create_puzzle_from_string("112\n322\n");
  puzzle * p2 = create_puzzle_from_string("121\n322\n");

  printf("p1 = %d\n", have_seen_isomorph(p1));
  printf("p2 = %d\n", have_seen_isomorph(p2));
  canonize_puzzle(p1);
  canonize_puzzle(p2);
  print_puzzle(p1);
  printf("==================\n");
  print_puzzle(p2);

  return 0;

  puzzle * p = create_puzzle(2,6);

  int num = 0;
  int num_usp = 0;
  int num_unique = 0;
  int num_unique_usp = 0;
  
  for (puzzle_row r1 = 0; r1 < p -> max_row; r1++){

    p -> puzzle[0] = r1;
    p -> s = 1;
    if (have_seen_isomorph(p)){
      continue;
    }
    p -> s = 2;
    
    for (puzzle_row r2 = r1 + 1; r2 < p -> max_row; r2++){
      num++;

      p -> puzzle[1] = r2;
      if (IS_USP == check(p)) num_usp++;
      if (!have_seen_isomorph(p)){
	assert(have_seen_isomorph(p));
	num_unique++;
	if (IS_USP == check(p)){
	  num_unique_usp++;
	  canonize_puzzle(p);
	  assert(IS_USP == check(p));
	  assert(have_seen_isomorph(p));
	  print_puzzle(p);
	  printf("----------------\n");
	}
      }
    }
  }
  printf("Unique %d / %d\n", num_unique, num);
  printf("SUSP   %d / %d\n", num_unique_usp, num_usp);
  
  
  return 0;
}
