#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "usp.h"
#include "puzzle.h"
#include "usp_bi.h"


#define MAX_S 100

bool puzzle_has_at_least_n_two_columns(puzzle_row U[], int s, int k, int n){

  int found = 0;
  for (int c = 0; c < k; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < s; r++){
      counts[get_column_from_row(U[r], c) - 1]++;
    }

    int missing = (counts[0] == 0) + (counts[1] == 0) + (counts[2] == 0);
    if (missing >= 1)
      found++;
  }

  return found >= n;
  
}


int brute_force(puzzle_row U[], int s, int k, bool skip[], int skipping, int max_r, int n){

  bool local_skip[max_r];
  memcpy(local_skip, skip, sizeof(bool) * max_r);

  int ret = s;
 
  for (U[s] = 0; U[s] < max_r; U[s]++){

    if (local_skip[U[s]]) continue;

    if (puzzle_has_at_least_n_two_columns(U, s+1, k, n) && check(U, s+1, k)){

      int res = brute_force(U, s+1, k, local_skip, skipping, max_r, n);
      ret = (res > ret ? res : ret);

    } else {
      local_skip[U[s]] = true;
      skipping++;
    }
    
  }

  return ret;

}

int main(int argc, char *argv[])
{

  if (argc != 5 && argc != 6){
    
    fprintf(stderr,"usage: usp_greedy <s> <k> <two-elt-cols> <stride> [start-usp]\n");
    return -1;
  }

  int s = atoi(argv[1]);
  int k = atoi(argv[2]);
  int special_num = atoi(argv[3]);
  int stride_init = atoi(argv[4]);
  int stride = (stride_init == 0 ? 1 : stride_init);
    
  int max_r = (int)pow(3,k);
  int counts[max_r];
  bool skip[max_r];
  int skipping = 0;
  bzero(skip, max_r * sizeof(bool));

  long seed = time(NULL);
  printf("seed = %ld\n", seed);
  srand48(seed);

  int max = 2;
  int r = 0;
  
  puzzle_row U[MAX_S];
  
  if (argc == 6) {
    puzzle * p = create_puzzle_from_file(argv[5]);

    if (p == NULL) {
      fprintf(stderr,"Error: unable to locate puzzle file.\n");
      return -2;
    }
    bzero(U, sizeof(puzzle_row) * MAX_S);
    memcpy(U, p -> puzzle, p -> row * sizeof(puzzle_row));
    r = p -> row;
    printU(U,r,k);
    //destroy_puzzle(p);
  }

  for ( ; r < s && max > 1; r++){

    bzero(counts, max_r * sizeof(int));
    max = 0;
    int max_num = 0;

    // First new row.
    for (U[r] = 0; U[r] < max_r; U[r]++){

      printf("\r%8.2f%%", U[r] / (double)max_r * 100.0);
      fflush(stdout);
      
      if (skip[U[r]]) continue;

      if (puzzle_has_at_least_n_two_columns(U,r+1,k,special_num) && check(U, r + 1, k)){

	counts[U[r]]++;
	if (counts[U[r]] > max) {
	  max = counts[U[r]];
	  max_num = 1;
	} else if (counts[U[r]] == max) {
	  max_num++;
	}
	
	if ((r + 1) < s) {
	  int step = 1;

	  // Second new row.
	  for (U[r+1] = 0; U[r+1] < max_r; U[r+1]++){
	    
	    if (!skip[U[r+1]] && puzzle_has_at_least_n_two_columns(U,r+2,k,special_num)){
	      if (step != stride) {
		step++;
	      } else {
		step = 1;		
		
		if (check(U, r+2, k)) {
		  counts[U[r]]++;
		  if (counts[U[r]] > max) {
		    max = counts[U[r]];
		    max_num = 1;
		  } else if (counts[U[r]] == max) {
		    max_num++;
		  }
		  /* Third new row
		  if ((r + 2) < s){
		    for (U[r+2] = 0; U[r+2] < max_r; U[r+2]++){
		      
		      if (!skip[U[r+2]] && puzzle_has_at_least_n_two_columns(U,r+3,k,special_num)){
			
			if (check(U, r+3, k)) {
			  counts[U[r]]++;
			  if (counts[U[r]] > max) {
			    max = counts[U[r]];
			    max_num = 1;
			  } else if (counts[U[r]] == max) {
			    max_num++;
			  }
			}
		      }
		    }
		  }
		  */
			
		}
	      }
	    }
	  }
	}
      } else {
	skip[U[r]] = true;
	skipping++;
      }
    }
    printf("\r%2d: max = %5d, stride = %5d, special = %2d, skipping = %7.4f%%\n", r+1, max, stride, special_num, skipping / (double)max_r * 100.0);
    
    int choice = lrand48() % max_num;
    int found = 0;
    if (stride_init != 0) {
      stride = (int)(sqrt(max * stride) / 4.0);
      if (stride < 1)
	stride = 1;
    }

    //if (max < special_num*2 && special_num > 0)
    //      special_num--;
    
    if (max >= 1){
      for (U[r] = 0; true ; U[r]++){ 
	if (counts[U[r]] == max) {
	  found++;
	  if (found > choice)
	    break;
	}
      }
    }
    
  }

  //printf("best = %d\n", brute_force(U, r, k, skip, skipping, max_r, special_num));
  
  printU(U,r,k);
  if (check(U,r,k)){
    printf("is a strong USP.\n");
  } else {
    printf("is NOT a strong USP.\n");
  }
  return 0;

}
