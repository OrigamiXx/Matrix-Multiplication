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


int explore_twist(puzzle * p1, puzzle * p2) {

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


  
  int k1 = p1 -> column;
  int s1 = p1 -> row;
  int k2 = p2 -> column;
  int s2 = p2 -> row;
  int dk = k1 - p2 -> column;
  long num_arb = (long)pow(3, s2 * dk);
  assert((long)s2 * (long)dk <= (long)19);

  int count = 0;
  
  puzzle * p = create_puzzle(s1 + s2, k1 + 1);
  int * puz = p -> puzzle;

  // Copy p1 into p.
  for (int c = 0; c < k1; c++){
    for (int r = 0; r < s1; r++){
      puz[r] = set_entry_in_row(puz[r], c, get_column_from_row(p1 -> puzzle[r], c));      
    }
  }

  int iter = 0;
  perm * pi = create_perm_identity(k1);

  bool stop = false;
  for ( ; !stop ; stop = !next_perm(pi)){

    iter++;
    printf("\n%d: ", iter);
    print_perm_cycle(pi);
    
    //print_puzzle(p);
    
    // Copy p2 into p according to pi.
    for (int c = 0; c < k2; c++){
      for (int r = 0; r < s2; r++){
	puz[r + s1] =
	  set_entry_in_row(puz[r + s1], apply_perm(pi, c), get_column_from_row(p2 -> puzzle[r], c)); 
      }
    }
    
    for (int arb = 0; arb < num_arb; arb++) {
      //for (int arb = 1; arb <= 3; arb++) {
      
      printf("\rarb = %d / %ld",arb, num_arb);
      fflush(stdout);
      
      int i = 0;
      for (int c = k2; c < k1; c++){
	for (int r = 0; r < s2; r++){
	  puz[r + s1] =
	    set_entry_in_row(puz[r + s1], apply_perm(pi,c),
			     get_column_from_row(arb, i)
			     //arb
			     );
	  i++;
	}
      }
      
      for (int x = 1; x <= 3; x++){
	
	for (int r = 0; r < s1; r++){
	  puz[r] = set_entry_in_row(puz[r], k1, x);
	}
	
	
	for (int y = 1; y <= 3; y++){
	  
	  if (x == y) continue;
	  
	  for (int r = 0; r < s2; r++){
	    puz[r + s1] = set_entry_in_row(puz[r + s1], k1, y);
	  }
	  
	  //if (check_row_triples(puz, p -> row, p -> column));
	  
	  // Puzzle p is filled.
	  if (check(puz, p -> row, p -> column)) {
	    //printf("Constructed a strong USP!\n");
	    count++;
	    //print_puzzle(p);
	    //printf("\n");
	    destroy_perm(pi);
	    destroy_puzzle(p);
	    return count;
	  }
	}
      }
    }
  }
  
  //printf("Constructed %d strong USPs (%d, %d)\n", count, s1 + s2, k1 + 1);
  destroy_perm(pi);
  destroy_puzzle(p);
  return count;
  
}


bool puzzle_has_two_column(puzzle * p){

  bool found = false;
  for (int c = 0; c < p -> column; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < p -> row; r++){
      counts[get_column_from_row(p -> puzzle[r], c) - 1]++;
    }

    int missing = (counts[0] == 0) + (counts[1] == 0) + (counts[2] == 0);
    if (missing == 2)
      return false;
    if (missing == 1)
      found = true;
  }

  return found;
  
}

void random_usp(puzzle * p){

  int s = p -> row;
  int k = p -> column;
  
  randomize_puzzle(p);
  while (true) {
    
    if (//puzzle_has_two_column(p) &&
	check(p -> puzzle, s, k)) {
      //printf("Found %d-by-%d strong.\n", s, k);
      return;
    }

    randomize_puzzle(p);
  }

}

bool random_twist(puzzle * p, puzzle * p1, puzzle * p2, int iter){

  int s = p -> row;
  int k = p -> column;
  int * puz = p -> puzzle;
  int k1 = p1 -> column;
  int s1 = p1 -> row;
  int k2 = p2 -> column;
  int s2 = p2 -> row;
  int dk = k1 - p2 -> column;

  long num_last = 6;
  long num_arb = 3 + k2;
  //long num_arb = k2;
  //long num_arb = (long)pow(3, s2 * dk);
  long num_perm = 1;
  for (int i = 1; i <= k; i++) num_perm *= i;
  
  for (int i = 0; i < iter; i++){

    long last = lrand48() % num_last;
    long arb = lrand48() % num_arb;
    long perm_id = lrand48() % num_perm;

    int x, y;
    if (last < 2) {
      x = 1;
      y = (last == 0 ? 2 : 3);
    } else if (last < 4){
      x = 2;
      y = (last == 2 ? 1 : 3);
    } else {
      x = 3;
      y = (last == 4 ? 1 : 2);
    }

    perm * pi = create_perm_identity(k1);
    for (int j = 0; j < perm_id; j++)
      next_perm(pi);

    // Copy p1 into p.
    for (int c = 0; c < k1; c++){
      for (int r = 0; r < s1; r++){
	puz[r] = set_entry_in_row(puz[r], c, get_column_from_row(p1 -> puzzle[r], c));      
      }
    }

    // Copy p2 into p according to pi.
    for (int c = 0; c < k2; c++){
      for (int r = 0; r < s2; r++){
	puz[r + s1] =
	  set_entry_in_row(puz[r + s1], apply_perm(pi, c), get_column_from_row(p2 -> puzzle[r], c)); 
      }
    }

    int id = 0;
    for (int c = k2; c < k1; c++){
      for (int r = 0; r < s2; r++){
	puz[r + s1] =
	  set_entry_in_row(puz[r + s1], apply_perm(pi,c),
			   //get_column_from_row(arb, id)
			   (arb < 3 ? arb + 1 : get_column_from_row(p2 -> puzzle[r], arb - k2))
			   //get_column_from_row(p2 -> puzzle[r], arb)
			   );
	id++;
      }
    }

    for (int r = 0; r < s1; r++){
      puz[r] = set_entry_in_row(puz[r], k1, x);
    }

    for (int r = 0; r < s2; r++){
      puz[r + s1] = set_entry_in_row(puz[r + s1], k1, y);
    }

    destroy_perm(pi);

    if (check(puz, s, k))
      return true;
  }

  return false;
}

int total = 0;
int num_done = 0;

void random_constructed_usp(puzzle * p, int iter){

  int s = p -> row;
  int k = p -> column;
  
  if (s <= 2) {
    random_usp(p);
  } else {
    puzzle * p1;
    puzzle * p2;
    int s1, s2, k1, k2;
    if (k == 3) {
      s1 = 2;
      k1 = 2;
      s2 = 1;
      k2 = 1;
    } else if (k == 4){
      s1 = 3;
      k1 = 3;
      s2 = s - s1;
      k2 = 2;
    } else if (k == 5){
      s1 = 5;
      k1 = 4;
      s2 = s - s1;
      k2 = 3;
    } else if (k == 6){
      s1 = 8;
      k1 = 5;
      s2 = s - s1;
      k2 = 4;
    }
    p1 = create_puzzle(s1, k1);
    p2 = create_puzzle(s2, k2);
    //printf("%dx%d %dx%d\n",s1,k1,s2,k2);
    assert(p->row == p1->row + p2->row);

    bool found = false;
    int tries = 0;
    while (!found) {
      random_constructed_usp(p1, iter);
      random_constructed_usp(p2, iter);

      found = random_twist(p, p1, p2, iter);
      tries++;
    }
    if (k >= 5) {
      total += tries;
      num_done++;
      printf("%d-by-%d: %d  average: %f\n", s, k, tries, total / (double)num_done);
    }
      
    destroy_puzzle(p1);
    destroy_puzzle(p2);
  }
  

}

int main(int argc, char * argv[]){

  if (argc != 6 && argc != 4 && argc != 3) {
    fprintf(stderr, "usp_construct <file1> <file2> | <s> <k> <iter> | <s1> <k1> <s2> <k2> <iter>\n");
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
  } else if (argc == 4) {

    int s = atoi(argv[1]);
    int k = atoi(argv[2]);
    int iter = atoi(argv[3]);

    puzzle * p = create_puzzle(s, k);

    srand48(time(NULL));

    for (int i = 0; i < 100; i++) {
      random_constructed_usp(p, iter);
      print_puzzle(p);
      printf("\n");
    }
    
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



    /*
    for (int r = 1; r <= 4 && r <= s1; r++)
      init_cache(r, k1);
    for (int r = 1; r <= 4 && r <= s2; r++)
      init_cache(r, k2);
    for (int r = 1; r <= 4 && r <= s1+s2; r++)
      init_cache(r, k1+1);
    */

    srand48(time(NULL));
    
    int found = 0;
    int total = 0;

    for (int i = 0; i < iter; i++){

      random_usp(p1);
      random_usp(p2);

      int ret = explore_twist(p1, p2);
      total += ret;
      if (ret > 0)
	found++;
      printf("Found = %d / %d\n", found, i+1);
      
    }

    printf("Found = %d / %d\n", found, iter);
    printf("Average count = %f \n", total / (double)iter);
    
    //destroy_puzzle(p1);
    //destroy_puzzle(p2);    
  }

  


    
  return 0;

}
