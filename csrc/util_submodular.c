#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"

// Returns the number of rows of the largest subpuzzle of p containing
// rows indexed > max_i that is a strong USP.
int rank(puzzle * p, int max_i){

  if (IS_USP == check(p)){
    // Should return at least s >= 1.
    return p -> s;
  }

  if (max_i < 0)
    return -1;
  
  puzzle * q = create_row_minor_puzzle(p, max_i);

  int a = rank(q, max_i - 1);
  destroy_puzzle(q);
  if (a == p -> s - 1)
    return p -> s - 1;

  int b = rank(p, max_i - 1);
  return MAX(a, b);

}

// Returns the number of rows of the largest subpuzzle of p that is a
// strong USP.
int rank(puzzle *p){
  return rank(p, p -> s - 1);
}

// Computes discrete derivative rank(row | p) = rank(p + row) - rank(p).
int derivative(puzzle * p, puzzle_row row) {

  puzzle * q = create_puzzle_from_puzzle(p, row);
  int result = rank(q) - rank(p);
  destroy_puzzle(q);
  
  return result;
    
}

int dependent_all_puzzles(puzzle * p, puzzle_row u, puzzle_row v,  int i){

  int res = 0;
  
  if (i == p -> s) {

    puzzle * q = create_puzzle_from_puzzle(p, v);

    int a = derivative(q, u);
    int b = derivative(p, u);

    destroy_puzzle(q);

    if (a != b)
      res = 1;  // Dependent
    if (a > b)  
      res = 2;  // Supermodular

    
    /*
    if (dependent) {
      printf("p:\n");
      print_puzzle(p);
      printf("q:\n");
      print_puzzle(q);
      printf("row:");
      print_row(u, p -> k);
      printf("\n");
      }*/
	
  } else {
    
    puzzle_row start = 0;
    if (i > 0)
      start = p -> puzzle[i-1] + 1;
    
    for (p -> puzzle[i] = start;  p -> puzzle[i] < p -> max_row && res < 2; p -> puzzle[i]++){
      
      res = MAX(res, dependent_all_puzzles(p, u, v, i+1));
      
    }
  }
  
  return res;
}

void dependency_degree(int max_s, int k, puzzle_row u, puzzle_row max_row, int * dependent_count, int * supermodular_count){

  *dependent_count = 0;
  *supermodular_count = 0;

  for (puzzle_row v = 0; v < max_row; v++){
    if (u == v) continue;

    int res = 0;
    for (int s = 1; s <= max_s && res < 2; s++){

      puzzle * p = create_puzzle(s, k);
      
      res = MAX(res, dependent_all_puzzles(p, u, v, 0));
      
      destroy_puzzle(p);
      
    }
    if (res >= 1) (*dependent_count)++;
    if (res >= 2) (*supermodular_count)++;
      
  }

}
  
void all_dependency_degrees(int max_s, int k){

  puzzle_row max_row = MAX_ROWS[k];

  for (puzzle_row u = 0; u < max_row; u++){
    int dependent_count = 0;
    int supermodular_count = 0;
    dependency_degree(max_s, k, u, max_row, &dependent_count, &supermodular_count);
    print_row(u, k);
    printf(": %d %d\n", dependent_count, supermodular_count);
  }

}


int main(int argc, char * argv[]){

  if (argc != 3){
    fprintf(stderr, "usage: util_submodular <s> <k>\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);

  srand48(time(NULL));
  
  puzzle * p = create_puzzle(s,k);
  randomize_puzzle(p);
  print_puzzle(p);
  printf("rank = %d\n", rank(p));
  destroy_puzzle(p);

  all_dependency_degrees(s, k);
  
  return 0;
}
