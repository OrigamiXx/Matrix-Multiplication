#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "3DM_to_SAT.h"

/*
 * Join two puzzles to form a new puzzle with one more column.
 * Unknown entries will be defaulted to 1.  Assumes that the width of
 * p1 is at least the width of p1.
 */
puzzle * join_puzzles(puzzle * p1, puzzle * p2) {

  int s1 = p1 -> s;
  int s2 = p2 -> s;
  int k1 = p1 -> k;
  int k2 = p2 -> k;
  
  puzzle * res = create_puzzle(s1 + s2, k1 + 1);

  for (int i = 0; i < res -> s; i++)
    if (i < s1)
      res -> puzzle[i] = p1 -> puzzle[i];
    else
      res -> puzzle[i] = (int)pow(3,res -> k - k2) * p2 -> puzzle[i - s1];
  
  return res;
}

void explore_joint(puzzle * p1, puzzle * p2){
  
  //puzzle * p1 = create_puzzle_from_file(argv[1]);
  //puzzle * p2 = create_puzzle_from_file(argv[2]);
  
  puzzle * joint = join_puzzles(p1, p2);

  print_puzzle(joint);

  int unknown_entries = p1 -> s + p2 -> s * (joint -> k - p2 -> k);

  printf("Unknown entries: %d\n", unknown_entries);

  puzzle_row max_iter = 1;
  for(int i = 0 ; i < unknown_entries ; i++){
    max_iter = max_iter * 3;
  }

  printf("Looping over all unknowns: %lu iterations.\n", max_iter);

  int num_found = 0;
  
  for (puzzle_row curr = 0 ; curr < max_iter ; curr++){

    int i = 0;
    for ( ; i < p1 -> s ; i++)
      set_entry(joint, i, joint -> k - 1, get_column_from_row(curr, i));

    for (int j = 0 ; j < p2 -> s ; j++) {
      for (int k = 0; k < joint -> k - p2 -> k ; k++){
	set_entry(joint, p1 -> s + j, k , get_column_from_row(curr, i));
	i++;
      }
    }

    if (curr % 97 == 0) {
      printf("\r %lu %d",curr, num_found);
      fflush(stdout);
    }
    check_t res = check_usp_bi(joint);
    //    printf("check_usp_bi:  %d\n", is_usp);
    if (res == IS_USP) {
      num_found++;
      printf("Found USP!\n");
      print_puzzle(joint);
      printf("\n");
    }
    //printf("curr = %d\n", curr);
    //print_puzzle(joint);
    //printf("\n");
  }
  
  //printf("check_usp:     %d\n", check_usp(p));
  //printf("check_usp_uni: %d\n", check_usp_uni(p -> puzzle, p -> row, p -> column));

  destroy_puzzle(joint);
  
}


int main(int argc, char * argv[]){

  if (argc != 4) {
    fprintf(stderr, "usp_exp <rows> <cols> <iter>\n");
    return -1;
  }

  int r = atoi(argv[1]);
  int c = atoi(argv[2]);
  int iter = atoi(argv[3]);
  
  puzzle * p = create_puzzle(r, c);

  int found = 0;
  for (int i = 0; i < iter ; i++){
    randomize_puzzle(p);
    bool curr_found = false;
    
    check_t res = check(p);
    curr_found = curr_found || (res == IS_USP);

    if (curr_found) {
      found++;
    }
    
  }

  printf("Found %d USP(s).\n", found);

  destroy_puzzle(p);

  return 0;

}
