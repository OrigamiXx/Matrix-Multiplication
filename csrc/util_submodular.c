#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"

// Returns the number of rows of the largest subpuzzle of p containing
// rows indexed > max_i that is a strong USP.
int sub_rank(puzzle * p, int max_i, int best){

  if (IS_USP == check(p)){
    // Should return at least s >= 1.
    return p -> s;
  }

  if (max_i < 0 || p -> s - 1 <= best)
    return -1;
  
  puzzle * q = create_row_minor_puzzle(p, max_i);

  int a = sub_rank(q, max_i - 1, best);
  destroy_puzzle(q);
  if (a == p -> s - 1)
    return p -> s - 1;

  int b = sub_rank(p, max_i - 1, MAX(best, a));
  return MAX(a, b);

}

// Returns the number of rows of the largest subpuzzle of p that is a
// strong USP.
int sub_rank(puzzle *p){
  return sub_rank(p, p -> s - 1, 0);
}

// Computes discrete derivative rank(row | p) = rank(p + row) - rank(p).
int derivative(puzzle * p, puzzle_row row) {

  puzzle * q = create_puzzle_from_puzzle(p, row);
  int result = sub_rank(q) - sub_rank(p);
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


int search_continuous_greedy(int k){

  puzzle_row n = MAX_ROWS[k];
  
  puzzle_row iters = n * n; //sqrt(n); //* n;
  double delta = 1.0 / iters;
  puzzle_row samples = n; //sqrt(n); //* n * n * n * n;

  int best_found = 0;
  
  double y[n];
  double w[n];
  double r[n];


  bzero(y, n * sizeof(double));
  bzero(w, n * sizeof(double));
  bzero(r, n * sizeof(double));
  
  for (puzzle_row i = 0; i < iters; i++){

    printf("\r %5lu / %5lu -> %d",i, iters, best_found);
    fflush(stdout);
    
    // Compute w by sampling according to y.
    for (puzzle_row j = 0; j < n; j++){

      w[j] = 0.0;
      for (puzzle_row m = 0; m < samples; m++){
	puzzle_row count = 0;
	for (puzzle_row g = 0; g < n; g++) {
	  r[g] = (drand48() <= y[g] ? 1 : 0);
	  if (r[g] == 1)
	    count++;
	}
	//printf("count = %lu\n",count);
	puzzle * p = create_puzzle(count, k);
	int h = 0;
	for (puzzle_row g = 0; g < n; g++)
	  if (r[g] == 1) 
	    p -> puzzle[h++] = g;

	int max_s = sub_rank(p);
	best_found = MAX(max_s, best_found);
	
	if (r[j] == 1)
	  w[j] += max_s;
	else
	  w[j] -= max_s;
	
	destroy_puzzle(p);
      }
      w[j] = w[j] / samples;
      
    }

    // Compute maximal independent set in p using w.  (XXX - Suppose to be maximum...)
    puzzle * p = create_puzzle(0, k);

    bool progress = true;
    while (progress) {

      progress = false;
      puzzle_row best = -1;
      for (puzzle_row j = 0; j < n; j++)
	if (r[j] != -1.0 && (best == -1 || w[j] > w[best])){
	  best = j;
          progress = true;
	}

      if (progress){
	r[best] = -1.0;

	puzzle * q = create_puzzle_from_puzzle(p, best);
	if (IS_USP == check(q)){
	  destroy_puzzle(p);
	  p = q;
	  best_found = MAX(p -> s, best_found);
	} else {
	  destroy_puzzle(q);
	}
	
      } 
      
    }

    destroy_puzzle(p);
    
    // Update y
    for (int j = 0; j < p -> s; j++){
      y[p -> puzzle[j]] += delta; 
    }
    
  }

  // XXX - Suppose to do randomized rounding...
  
  return best_found;

}


int search_random_greedy(int k){

  int iters = 1000;
  puzzle_row n = MAX_ROWS[k];
  
  puzzle_row w[n];

  int best_found = 0;
  int total = 0;
  
  for (int i = 0; i < iters; i++){

    printf("\r %5d / %5d -> %d  %f", i, iters, best_found, total / (double)i);
    fflush(stdout);

    for (puzzle_row j = 0; j < n; j++)
      w[j] = j;

    for (puzzle_row j = 0; j < n; j++) {
      puzzle_row m = (puzzle_row)(drand48() * n * n) % n;
      puzzle_row t = w[j];
      w[j] = w[m];
      w[m] = t;
    }

    
    // Compute maximal independent set in p using w.  (XXX - Suppose to be maximum...)
    puzzle * p = create_puzzle(0, k);

    int found = 0;
    for (puzzle_row j = 0; j < n; j++){	
	puzzle * q = create_puzzle_from_puzzle(p, w[j]);
	if (IS_USP == check(q)){
	  destroy_puzzle(p);
	  p = q;
	  found = p -> s;
	  printf("found = %d\n", found);
	  best_found = MAX(p -> s, best_found);
	} else {
	  destroy_puzzle(q);
	}
    } 
    total += found;
    
    destroy_puzzle(p);
  }

  return best_found;
}

int main(int argc, char * argv[]){

  if (argc != 3){
    fprintf(stderr, "usage: util_submodular <s> <k>\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);

  srand48(time(NULL));

  /*
  puzzle * p = create_puzzle(s,k);
  randomize_puzzle(p);
  print_puzzle(p);
  printf("sub_rank = %d\n", sub_rank(p));
  destroy_puzzle(p);
  */
  // Analyzes closeness to submodular.
  //all_dependency_degrees(s, k);

  //printf("search_continuous_greedy = %d\n", search_continuous_greedy(k));

  printf("search_random_greedy = %d\n", search_random_greedy(k));
  
  return 0;
}
