#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "usp_bi.h"

/*
 * Join two puzzles to form a new puzzle with one more column.
 * Unknown entries will be defaulted to 1.  Assumes that the width of
 * p1 is at least the width of p1.
 */
puzzle * join_puzzles(puzzle * p1, puzzle * p2) {

  int r1 = p1 -> row;
  int r2 = p2 -> row;
  int c1 = p1 -> column;
  int c2 = p2 -> column;

  puzzle * res = (puzzle *) (malloc(sizeof(puzzle)));
  res -> row = r1 + r2;
  res -> column = c1 + 1;
  res -> pi = create_perm_identity(res -> row); 
  res -> puzzle = (int *) malloc(sizeof(int *) * res -> row);

  for (int i = 0; i < res -> row; i++){
    if (i < r1)
      res -> puzzle[i] = p1 -> puzzle[i];
    else
      res -> puzzle[i] = p2 -> puzzle[i - r1]; //(int)pow(3,res -> column - c2) * p2 -> puzzle[i - r1];
  }
  
  return res;
}


int main(int argc, char * argv[]){


  if (argc != 3) {
    fprintf(stderr, "Must provide 2 puzzles to combine.\n");
    return -1;
  }

  puzzle * p1 = create_puzzle_from_file(argv[1]);
  puzzle * p2 = create_puzzle_from_file(argv[2]);

  if (p1 -> column < p2 -> column){
    puzzle * tmp = p1;
    p1 = p2;
    p2 = tmp;
  }

  printf("p1 = \n");
  print_puzzle(p1);
  printf("\n");
  printf("check_usp_bi: %d\n", check_usp_bi(p1 -> puzzle, p1 -> row, p1 -> column));

  printf("p2 = \n");
  print_puzzle(p2);
  printf("\n");
  printf("check_usp_bi: %d\n", check_usp_bi(p2 -> puzzle, p2 -> row, p2 -> column));
  
  puzzle * joint = join_puzzles(p1, p2);

  
  print_puzzle(joint);

  int unknown_entries = p1 -> row + p2 -> row * (joint -> column - p2 -> column);

  printf("Unknown entries: %d\n", unknown_entries);

  int max_iter = 1;
  for(int i = 0 ; i < unknown_entries ; i++){
    max_iter = max_iter * 3;
  }

  printf("Looping over all unknowns: %d iterations.\n", max_iter);

  int num_found = 0;
  
  for (int curr = 0 ; curr < max_iter ; curr++){

    int i = 0;
    for ( ; i < p1 -> row ; i++) 
      joint -> puzzle[i] =
	set_entry_in_row(joint -> puzzle[i], joint -> column - 1, get_column_from_row(curr, i));

    for (int j = 0 ; j < p2 -> row ; j++) {
      for (int k = 0; k < joint -> column - p2 -> column ; k++){
	//printf("get_column_from_row(%d,%d) = %d\n", curr, i, get_column_from_row(curr, i));
	joint -> puzzle[p1 -> row + j] =
	  set_entry_in_row(joint -> puzzle[p1 -> row + j], k + p2 -> column, get_column_from_row(curr, i));

	i++;
      }
    }

    if (curr % 97 == 0) {
      printf("\r %d %d",curr, num_found);
      fflush(stdout);
    }
    int is_usp = check_usp_bi(joint -> puzzle, joint -> row, joint -> column);
    //    printf("check_usp_bi:  %d\n", is_usp);
    if (is_usp) {
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


  destroy_puzzle(p1);
  destroy_puzzle(p2);
  destroy_puzzle(joint);
  
  
  return 0;

}
