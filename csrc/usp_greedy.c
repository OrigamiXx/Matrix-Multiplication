#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "usp.h"
#include "puzzle.h"


#define MAX_S 100

bool puzzle_has_at_least_n_two_columns(puzzle * p, int n){

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

  return found >= n;
  
}


int brute_force(puzzle * p, bool skip[], int skipping, int max_row, int n){

  bool local_skip[max_row];
  memcpy(local_skip, skip, sizeof(bool) * max_row);
  int s = p -> s;
  int ret = s;

  puzzle_row * U = p -> puzzle;
  p -> s++;
  
  for (U[s] = 0; U[s] < max_row; U[s]++){

    if (local_skip[U[s]]) continue;

    if (puzzle_has_at_least_n_two_columns(p, n) && IS_USP == check(p)){

      int res = brute_force(p, local_skip, skipping, max_row, n);
      ret = (res > ret ? res : ret);

    } else {
      local_skip[U[s]] = true;
      skipping++;
    }
    
  }

  return ret;

}




puzzle * create_usp_greedy(int s, int k, int stride_init, int special, puzzle * start_p, bool verbose){

  int stride = (stride_init == 0 ? 1 : stride_init);

  int max = 2;
  int r = 0;

  puzzle * p = create_puzzle(s,k);
  if (p == NULL)
    return NULL;
  int max_row = p -> max_row;

  int counts[max_row];
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


    
    bzero(counts, max_row * sizeof(int));
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

      if (puzzle_has_at_least_n_two_columns(p, special) && IS_USP == check(p)){


	
	counts[puz[r]]++;
	if (counts[puz[r]] > max) {
	  max = counts[puz[r]];
	  max_num = 1;
	} else if (counts[puz[r]] == max) {
	  max_num++;
	}
	
	if ((r + 1) < s) {
	  int step = 1;

	  // Second new row.
	  for (puz[r+1] = 0; puz[r+1] < max_row; puz[r+1]++){
	    p -> s = r + 2;
	    
	    if (!skip[puz[r+1]] && puzzle_has_at_least_n_two_columns(p, special)){
	      if (step != stride) {
		step++;
	      } else {
		step = 1;		
		
		if (IS_USP == check(p)) {
		  counts[puz[r]]++;
		  if (counts[puz[r]] > max) {
		    max = counts[puz[r]];
		    max_num = 1;
		  } else if (counts[puz[r]] == max) {
		    max_num++;
		  }
		}
	      }
	    }
	  }
	}
	/*printf("density = %8.4f%%, count = %d\n",
	  count_tdm(p) / pow(p -> s, 3) * 100.0, counts[puz[r]]);*/
	
      } else {
	skip[puz[r]] = true;
	skipping++;
      }
      
    }

    p -> s = r;

    int choice = lrand48() % max_num;
    int found = 0;
    
    //if (max < special*2 && special > 0)
    //      special--;
    
    if (max >= 1){
      for (puz[r] = 0; true ; puz[r]++){ 
	if (counts[puz[r]] == max) {
	  found++;
	  if (found > choice)
	    break;
	}
      }
    }

    if (verbose)
      printf("\r%2d: max = %5d, stride = %5d, special = %2d, 3DM density = %8.4f%%, skipping = %8.4f%%\n",
	     r+1, max, stride, special, (count_tdm(p) - p -> s) / pow(p -> s, 3) * 100.0,
	     skipping / (double)max_row * 100.0);    

    if (stride_init != 0) {
      stride = (int)(sqrt(max * stride) / 4.0);
      if (stride < 1)
	stride = 1;
    }
    
  }

  //printf("best = %d\n", brute_force(puz, r, k, skip, skipping, max_r, special));
  p -> s = r;
  
  if (verbose) {
    
    print_puzzle(p);
    arrange_puzzle(p);
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

  puzzle * p = create_usp_greedy(s, k, stride_init, special, start_p, true);

  if (start_p != NULL)
    destroy_puzzle(start_p);

  if (p != NULL)
    destroy_puzzle(p);


  for (int i = 0; i < 100; i++){

    p = create_usp_greedy(s, k, special);
    //print_puzzle(p);
    //    printf("\n");
    arrange_puzzle(p);
    //assert(IS_USP == check(p));
    print_puzzle(p);
    printf("\n");
    
  }

  return 0;
}
