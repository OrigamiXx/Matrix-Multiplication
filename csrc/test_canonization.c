// Simple tester for the puzzle module.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "checker.h"
#include "constants.h"
#include "puzzle.h"
#include "canonization.h"

void verify_isomorphs(const char * s1, const char * s2, bool isomorphic){

  reset_isomorphs();
  
  puzzle * p1 = create_puzzle_from_string((char *)s1);
  puzzle * p2 = create_puzzle_from_string((char *)s2);

  assert(!have_seen_isomorph(p1,true));
  assert((isomorphic ? have_seen_isomorph(p2,true) : !have_seen_isomorph(p2,true)));
  canonize_puzzle(p1);
  canonize_puzzle(p2);
  assert(have_seen_isomorph(p1,true));
  assert(have_seen_isomorph(p2,true));
  
  destroy_puzzle(p1);
  destroy_puzzle(p2);

}

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
    if (!have_seen_isomorph(p,true)){
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

  verify_isomorphs("112\n322\n","121\n322\n", true);
  verify_isomorphs("112\n322\n","121\n222\n", false);
  verify_isomorphs("11\n32\n","13\n22\n", true);

  assert(are_isomorphs(create_puzzle_from_string((char *)"112\n322\n"),
		       create_puzzle_from_string((char *)"121\n322\n")));
  
  reset_isomorphs();


  int s = 8;
  int k = 5;
  puzzle * p = create_puzzle(s, k);
  
  int num = 0;
  int num_usp = 0;
  int num_unique = 0;
  int num_unique_usp = 0;
  
  for (puzzle_row r1 = 0; r1 < p -> max_row; r1++){
    
    p -> puzzle[0] = r1;
    
    p -> s = 1;
    if (have_seen_isomorph(p,true)) continue;
    p -> s = 2;

    for (puzzle_row r2 = r1+1; r2 < p -> max_row; r2++){

      
      p -> puzzle[1] = r2;

      p -> s = 2;
      if (NOT_USP == check(p)) continue;
      if (have_seen_isomorph(p,true)) continue;
      p -> s = 3;

      for (puzzle_row r3 = r2+1; r3 < p -> max_row; r3++){
	num++;
	printf("\r%14.8f%%", (float)(r1 * p -> max_row * p -> max_row + r2 * p -> max_row + r3) / (p -> max_row * p -> max_row * p -> max_row) * 100.0);
	fflush(stdout);
	
	p -> puzzle[2] = r3;
	
	if (IS_USP == check(p)) num_usp++;
	if (!have_seen_isomorph(p,true)){
	  assert(have_seen_isomorph(p,true));
	  num_unique++;
	  if (IS_USP == check(p)){
	    num_unique_usp++;
	    assert(IS_USP == check(p));
	    assert(have_seen_isomorph(p,true));
	    //print_puzzle(p);
	    //printf("----------------\n");
	  }
	}
      }
    }
  }
  
  printf("\r  100.00000000%%\n");
  printf("Num puzzles: %ld\n", MAX_ROWS[k] * MAX_ROWS[k] * MAX_ROWS[k] * MAX_ROWS[k] * MAX_ROWS[k]);
  printf("Unique %d / %d\n", num_unique, num); 
  printf("SUSP   %d / %d\n", num_unique_usp, num_usp);
  printf("Stored isomorphs %ld\n", get_num_isomorphs());
  
  return 0;
}


