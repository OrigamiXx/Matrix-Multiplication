#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "usp_bi.h"
#include <time.h>
#include "3DM_to_SAT.h"

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
  res -> puzzle = (int *) malloc(sizeof(int *) * res -> row);

  for (int i = 0; i < res -> row; i++){
    if (i < r1)
      res -> puzzle[i] = p1 -> puzzle[i];
    else
      res -> puzzle[i] = (int)pow(3,res -> column - c2) * p2 -> puzzle[i - r1];
  }
  
  return res;
}

void explore_joint(puzzle * p1, puzzle * p2){
  
  //puzzle * p1 = create_puzzle_from_file(argv[1]);
  //puzzle * p2 = create_puzzle_from_file(argv[2]);
  
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
	  set_entry_in_row(joint -> puzzle[p1 -> row + j], k , get_column_from_row(curr, i));

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

  destroy_puzzle(joint);
  
}


bool explore_twist(puzzle * p1, puzzle * p2) {

  /*
    +-----+x
    |     |x
    | p1  |x
    |     |x
    +-----+x
    +----+*y
    | p2 |*y
    +----+*y

    Assumes p2 is not wider than p1.  Try all alignments of the
    columns of p2 with p1.  Fill in missing short column arbitrarily.
    Add new column that distinguishes original puzzles.
  */

  puzzle * p = create_puzzle(p1 -> row + p2 -> row, p1 -> column + 1);
  int * puz = p -> puzzle;
  
  int k = p1 -> column;
  int dk = k - p2 -> column;
  long num_arb = (long)pow(3, p2 -> row * dk);
  assert((long)(p2 -> row) * (long)dk <= (long)19);
  bool stop = false;
  int count = 0;

  // Copy p1 into p.
  for (int c = 0; c < k; c++){
    for (int r = 0; r < p1 -> row; r++){
      puz[r] = set_entry_in_row(puz[r], c , get_column_from_row(p1 -> puzzle[r], c));      
    }
  }

  int iter = 0;
  for (perm * pi = create_perm_identity(k); !stop ; next_perm(pi)){

    iter++;
    //printf("\n%d: ", iter);
    //print_perm_cycle(pi);

    //print_puzzle(p);
    
    // Copy p2 into p according to pi.
    for (int c = 0; c < p2 -> column; c++){
      for (int r = 0; r < p2 -> row; r++){
	puz[r + p1 -> row] =
	  set_entry_in_row(puz[r + p1 -> row], apply_perm(pi,c), get_column_from_row(p2 -> puzzle[r], c)); 
      }
    }
    
    for (int arb = 0; arb < num_arb; arb++){
      //printf("\rarb = %d / %ld",arb, num_arb);
      //fflush(stdout);

      int i = 0;
      for (int c = p2 -> column; c < k; c++){
	for (int r = 0; r < p2 -> row; r++){
	  puz[r + p1 -> row] = 
	    set_entry_in_row(puz[r + p1 -> row], apply_perm(pi,c), get_column_from_row(arb, i));
	  i++;
	}
      }
      
      for (int x = 1; x <= 3; x++){

	for (int r = 0; r < p1 -> row; r++){
	  puz[r] = set_entry_in_row(puz[r], k, x);
	}
	
	for (int y = 1; y <= 3; y++){

	  if (x == y) continue;
	  
	  for (int r = 0; r < p2 -> row; r++){
	    puz[p1 -> row + r] = set_entry_in_row(puz[p1 -> row + r], k, y);
	  }

	  //if (check_row_triples(puz, p -> row, p -> column));
	  
	  // Puzzle p is filled.
	  if (check(puz, p -> row, p -> column)) {
	    //printf("Constructed a strong USP!\n");
	    count++;
	  }

	  //print_puzzle(p);
	  //printf("\n");
	  
	}
      }
    }

    if (is_last_perm(pi))
      stop = true;
  }
    
  //printf("Constructed %d strong USPs (%d, %d)\n", count, p1 -> row + p2 -> row, k + 1);

  return (count > 0);
  
}


void random_usp(puzzle * p, int s, int k){
  
  randomize_puzzle(p);
  while (true) {

    //if (check_row_pairs(p -> puzzle, s, k)){
    //  if (check_row_triples(p -> puzzle, s, k)){
    if (check(p -> puzzle, s, k))
      return;
    //   }
    //}
    randomize_puzzle(p);
  }

}
  
int main(int argc, char * argv[]){

  if (argc != 6 && argc != 3) {
    fprintf(stderr, "usp_construct <file1> <file2> | <s1> <k1> <s2> <k2> <iter>\n");
    return -1;
  }

  if (argc == 3){
    puzzle * p1 = create_puzzle_from_file(argv[1]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle One does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    puzzle * p2 = create_puzzle_from_file(argv[2]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle Two does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    if (!check(p1 -> puzzle, p1 -> row, p1 -> column)){
      fprintf(stderr, "Warning: Puzzle One is not a strong USP.\n");
    }
    
    if (!check(p2 -> puzzle, p2 -> row, p2 -> column)){
      fprintf(stderr, "Warning: Puzzle Two is not a strong USP.\n");
    }
    
    // Make p1 the wider of the two.
    if (p1 -> column < p2 -> column){
      puzzle * tmp = p1;
      p1 = p2;
      p2 = tmp;
    }
    
    printf("Initialized.\n");
    
    explore_twist(p1,p2);
    
    printf("Completed.\n");
    
    destroy_puzzle(p1);
    destroy_puzzle(p2);
  } else if (argc == 6){

    int s1 = atoi(argv[1]);
    int k1 = atoi(argv[2]);
    int s2 = atoi(argv[3]);
    int k2 = atoi(argv[4]);
    int iter = atoi(argv[5]);

    if (k1 < k2){
      int tmpk = k1;
      int tmps = s1;
      k1 = k2;
      s1 = s2;
      k2 = tmpk;
      s2 = tmps;
    }
    
    puzzle * p1 = create_puzzle(s1,k1);
    puzzle * p2 = create_puzzle(s2,k2);

    int found = 0;

    /*
    for (int r = 1; r <= 4 && r <= s1; r++)
      init_cache(r, k1);
    for (int r = 1; r <= 4 && r <= s2; r++)
      init_cache(r, k2);
    for (int r = 1; r <= 4 && r <= s1+s2; r++)
      init_cache(r, k1+1);
    */

    srand48(time(NULL)* 997);
    
    for (int i = 0; i < iter; i++){

      random_usp(p1, s1, k1);
      random_usp(p2, s2, k2);

      if (explore_twist(p1,p2))
	found++;

    }

    printf("Found = %d / %d\n",found, iter);

    destroy_puzzle(p1);
    destroy_puzzle(p2);    
  }

  


    
  return 0;

}
