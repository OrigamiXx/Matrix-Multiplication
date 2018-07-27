#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "checker.h"
#include "puzzle.h"
#include "canonization.h"

#define MAX_S 100
#define STRIDE_FACTOR 8.0

int count_special_columns(puzzle * p){

  int s = p -> s;
  int k = p -> k;

  int found = 0;
  for (int c = 0; c < k; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < s; r++){
      counts[get_entry(p, r, c) - 1]++;
    }

    int missing = (counts[0] == 0) + (counts[1] == 0) + (counts[2] == 0);
    if (missing >= 1)
      found++;
  }

  return found;
  
}

bool puzzle_has_at_least_n_special(puzzle * p, int n){

  return count_special_columns(p) >= n;
  
}

int brute_force_greedy(puzzle * p, bool skip[], int skipping, int best){

  static unsigned long call = 0;
  static unsigned int min_since_best = 0;
  call++;
  printf("\r%lu\t%2d\t%2d", call, p -> s, min_since_best);
  fflush(stdout);
  
  //if (have_seen_isomorph(p, true))
  //    return best;
  
  bool local_skip[p -> max_row];
  memcpy(local_skip, skip, sizeof(bool) * p -> max_row);
  int s = p -> s;
  if (s > best) {
    best = s;
    min_since_best = best;
    printf("\nbest = %d\n", best);
  } 
    
  puzzle_row * U = p -> puzzle;
  
  for (U[s] = (s > 0 ? (U[s-1] + 1) : 0); U[s] < p -> max_row; U[s]++){

    if (local_skip[U[s]]) continue;

    p -> s = s + 1;
    if (IS_USP == check(p) && !have_seen_isomorph(p, true)){

      int res = brute_force_greedy(p, local_skip, skipping, best);
      if (res > best){
	best = res;
	min_since_best = best;
	printf("\nbest = %d\n", best);
      } 

    } else {
      local_skip[U[s]] = true;
      skipping++;
    }

    min_since_best = (s < min_since_best ? s : min_since_best);
    
  }

  return best;

}




puzzle * create_usp_greedy(int s, int k, int stride_init, int special, puzzle * start_p, bool verbose){

  int stride = (stride_init == 0 ? 1 : stride_init);

  int max = 2;
  int r = 0;

  puzzle * p = create_puzzle(s,k);
  if (p == NULL)
    return NULL;
  int max_row = p -> max_row;

  int fitness[max_row];
  bool skip[max_row];
  int skipping = 0;
  bzero(skip, max_row * sizeof(bool));
  
  if (start_p != NULL) {
    memcpy(p -> puzzle, start_p, start_p -> s * sizeof(int));
    if (verbose)
      print_puzzle(start_p);
  }

  puzzle_row * puz = p -> puzzle;
  
  for ( ; r < s && max > 1; r++){
    
    bzero(fitness, max_row * sizeof(int));
    max = 0;
    int max_num = 0;

    // First new row.
    for (puz[r] = 0; puz[r] < max_row; puz[r]++){
      p -> s = r + 1;
      
      if (verbose) {
	printf("\r%8.2f%%", puz[r] / (double)max_row * 100.0);
	fflush(stdout);
      }
	
      if (skip[puz[r]]) continue;

      bool seen = have_seen_isomorph(p, true);
      
      if (puzzle_has_at_least_n_special(p, special) && !seen && IS_USP == check(p)){

	int tdm_lower_thresh = 300;
	int tdm_upper_thresh = 27;
	
	if (r < tdm_lower_thresh || r > tdm_upper_thresh){
	  fitness[puz[r]]++;
	} else {
	  invalidate_tdm(p);
	  simplify_tdm(p);
	  fitness[puz[r]] = p -> s * p -> s * p -> s - count_tdm(p);
	  stride_init = 0;
	  stride = 1;
	}
	  
	if (fitness[puz[r]] > max) {
	  max = fitness[puz[r]];
	  max_num = 1;
	} else if (fitness[puz[r]] == max) {
	  max_num++;
	}

	if (r < tdm_lower_thresh || r > tdm_upper_thresh) {
	  if ((r + 1) < s) {
	    int step = 1;
	    
	    // Second new row.
	    for (puz[r+1] = 0; puz[r+1] < max_row; puz[r+1]++){
	      p -> s = r + 2;
	      
	      if (!skip[puz[r+1]] && puzzle_has_at_least_n_special(p, special)){
		if (step != stride) {
		  step++;
		} else {
		  step = 1;		
		  
		  if (IS_USP == check(p)) {
		    fitness[puz[r]]++;
		    if (fitness[puz[r]] > max) {
		      max = fitness[puz[r]];
		      max_num = 1;
		    } else if (fitness[puz[r]] == max) {
		      max_num++;
		    }
		  }
		}
	      }
	    }
	  }
	}

	/*printf("density = %8.4f%%, count = %d\n",
	  count_tdm(p) / pow(p -> s, 3) * 100.0, fitness[puz[r]]);*/
      } else {
	if (!seen) {
	  skip[puz[r]] = true;
	  skipping++;
	}
      }
      
    }

    p -> s = r;

    int choice = lrand48() % max_num;
    int found = 0;
    
    if (max >= 1){
      for (puz[r] = 0; true ; puz[r]++){ 
	if (fitness[puz[r]] == max) {
	  found++;
	  if (found > choice)
	    break;
	}
      }
    }

    invalidate_tdm(p);
    simplify_tdm(p);
    if (verbose)
      printf("\r                 \n");
      print_puzzle(p);
      printf("\r%2d: max = %5d, stride = %5d, special = %2d, 3DM density = %8.4f%%, skipping = %8.4f%%\n",
	     r+1, max, stride, count_special_columns(p), (count_tdm(p) - p -> s) / pow(p -> s, 3) * 100.0,
	     skipping / (double)max_row * 100.0);    

    if (stride_init != 0) {
      stride = (int)(sqrt(max * stride) / STRIDE_FACTOR);
      if (stride < 1)
	stride = 1;
    }
    
  }

  //printf("best = %d\n", brute_force(puz, r, k, skip, skipping, max_r, special));
  p -> s = r;
  
  if (verbose) {
    
    print_puzzle(p);
    //arrange_puzzle(p);
    printf("\n");
    print_puzzle(p);
    
    if (IS_USP == check(p)){
      printf("is a strong USP.\n");
    } else {
      printf("is NOT a strong USP.\n");
    }
  }

  if (r == s)
    return p;
  else {
    destroy_puzzle(p);
    return NULL;
  }

}

puzzle * create_usp_greedy(int s, int k, int special){

  puzzle * p = NULL;
  while (p == NULL) {
    p = create_usp_greedy(s, k, 1, special, NULL, false);
  }

  return p;
  
}


puzzle * create_usp_greedy(int s, int k){

  return create_usp_greedy(s, k, 0);
  
}


int main(int argc, char *argv[])
{

  if (argc != 5 && argc != 6){
    fprintf(stderr,"usage: usp_greedy <s> <k> <two-elt-cols> <stride> [start-usp]\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);
  int special = atoi(argv[3]);
  int stride_init = atoi(argv[4]);

  long seed = time(NULL);
  printf("seed = %ld\n", seed);
  srand48(seed);

  puzzle * start_p = NULL;
  if (argc == 6) {
    start_p = create_puzzle_from_file(argv[5]);
    if (start_p == NULL) {
      fprintf(stderr,"Error: unable to locate puzzle file.\n");
      return -2;
    }
  }

  //puzzle * p = create_usp_greedy(s, k, stride_init, special, start_p, true);

  puzzle * p = create_puzzle(s,k);
  p -> s = 0;
  bool skip[MAX_ROWS[k]];
  bzero(skip, MAX_ROWS[k] * sizeof(bool));
  int skipping = 0;
  printf("best = %d\n", brute_force_greedy(p, skip, skipping, 0));

  if (start_p != NULL)
    destroy_puzzle(start_p);

  if (p != NULL)
    destroy_puzzle(p);

  /*
  for (int i = 0; i < 100; i++){

    p = create_usp_greedy(s, k, special);
    //print_puzzle(p);
    //    printf("\n");
    arrange_puzzle(p);
    //assert(IS_USP == check(p));
    print_puzzle(p);
    printf("\n");
    
  }*/

  return 0;
}
