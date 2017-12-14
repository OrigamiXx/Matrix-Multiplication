#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "checker.h"
#include "puzzle.h"


#define MAX_S 100

puzzle * delete_puzzle_row(puzzle * p, int r){
  
  int s = p->s;
  int k = p->k;
  
  puzzle * new_p = create_puzzle(s - 1, k);

  p->puzzle[r] = p->puzzle[s-1];
  memcpy(new_p->puzzle, p->puzzle, (s-1)*sizeof(puzzle_row));
  
  destroy_puzzle(p);

  return new_p;
}

puzzle * reduce_puzzle_greedy(puzzle * p){


  int s = p->s;
  int k = p->k;


  int max_count = -1;
  int max_r = -1;
  
  for (int r = 0; r < s; r++){

    puzzle_row old_row = p->puzzle[r];

    int count = 0;
    
    for (p->puzzle[r] = 0; p->puzzle[r] < p->max_row; p->puzzle[r]++){

      if (check(p) == IS_USP){
	count++;
      }

    }

    p->puzzle[r] = old_row;
    
    if (count > max_count){
      max_r = r;
      max_count = count;
    }


  }
  
  return delete_puzzle_row(p, max_r);

}

puzzle * reduce_puzzle_random(puzzle * p){
  
  int s = p->s;

  return delete_puzzle_row(p, lrand48() % (s-1));

}

puzzle * extend_puzzle_once(puzzle * p){


  int s = p -> s;
  int k = p -> k;
  
  puzzle * new_p = create_puzzle(s + 1, k);


  memcpy(new_p->puzzle, p->puzzle, s * sizeof(puzzle_row));

  for (new_p->puzzle[s] = 0; new_p->puzzle[s] < new_p->max_row; new_p->puzzle[s]++){

    if (check(new_p) == IS_USP){
      return new_p;
    }

  }

  return NULL;

}


puzzle * extend_puzzle_once_greedy(puzzle * p){


  int s = p -> s;
  int k = p -> k;

  puzzle * new_p = create_puzzle(s + 2, k);


  int max_r = -1;
  int max_count = -1;

  memcpy(new_p->puzzle, p->puzzle, s * sizeof(puzzle_row));

  for (new_p->puzzle[s] = 0; new_p->puzzle[s] < new_p->max_row; new_p->puzzle[s]++){

    new_p -> s = s+1;

    int count = 0;
    
    if (check(new_p) != IS_USP)
      continue;


    for (new_p->puzzle[s+1] = new_p->puzzle[s]+1;
	 new_p->puzzle[s+1] < new_p->max_row;
	 new_p->puzzle[s+1]++){

      new_p -> s = s+2;
      if (check(new_p) == IS_USP){
	count++;
      }

    }

    if (count > max_count){
      max_count = count;
      max_r = new_p->puzzle[s];
    }
    
  }

  if (max_r != -1){
    new_p->puzzle[s] = max_r;
    new_p->s = s+1;
    return new_p;
  }
  
  return NULL;

}  


puzzle * extend_puzzle_twice(puzzle * p){


  int s = p -> s;
  int k = p -> k;
  
  puzzle * new_p = create_puzzle(s + 2, k);


  memcpy(new_p->puzzle, p->puzzle, s * sizeof(puzzle_row));
  
  for (new_p->puzzle[s] = 0; new_p->puzzle[s] < new_p->max_row; new_p->puzzle[s]++){

    new_p -> s = s+1;
    
    if (check(new_p) != IS_USP)
      continue;
    
    
    for (new_p->puzzle[s+1] = new_p->puzzle[s]+1;
	 new_p->puzzle[s+1] < new_p->max_row;
	 new_p->puzzle[s+1]++){

      new_p -> s = s+2;
      if (check(new_p) == IS_USP){
	return new_p;
      }
      
    }
    
  }

  return NULL;

}


puzzle * extend_puzzle_thrice(puzzle * p){


  int s = p -> s;
  int k = p -> k;
  
  puzzle * new_p = create_puzzle(s + 3, k);


  memcpy(new_p->puzzle, p->puzzle, s * sizeof(puzzle_row));
  
  for (new_p->puzzle[s] = 0; new_p->puzzle[s] < new_p->max_row; new_p->puzzle[s]++){

    new_p -> s = s+1;
    
    if (check(new_p) != IS_USP)
      continue;
    
    
    for (new_p->puzzle[s+1] = new_p->puzzle[s]+1;
	 new_p->puzzle[s+1] < new_p->max_row;
	 new_p->puzzle[s+1]++){

      
      new_p -> s = s+2;
      
      if (check(new_p) != IS_USP)
	continue;
      
      
      for (new_p->puzzle[s+2] = new_p->puzzle[s+1]+1;
	   new_p->puzzle[s+2] < new_p->max_row;
	   new_p->puzzle[s+2]++){

	new_p -> s = s+3;
	if (check(new_p) == IS_USP){
	  return new_p;
	}	
      }
    }
  }

  return NULL;

}


int main(int argc, char *argv[])
{

  if (argc != 2){
    fprintf(stderr,"usage: usp_ils <k>\n");
    return -1;
  }

  int k = atoi(argv[1]);

  
  long seed = time(NULL);
  printf("seed = %ld\n", seed);
  srand48(seed);

  puzzle * p = create_puzzle(0,k);
  
  while(true){
    puzzle * new_p = NULL;

    new_p = extend_puzzle_once_greedy(p);
    
    /*
    new_p = extend_puzzle_thrice(p);
    if (new_p == NULL){
      new_p = extend_puzzle_twice(p);
      if (new_p == NULL)
	new_p = extend_puzzle_once(p);
    }
    */
    
    if (new_p == NULL){
      // Do ILS.
      printf("stuck!\n");
      //print_puzzle(p);
      p = reduce_puzzle_greedy(p);
      //p = reduce_puzzle_greedy(p);
    } else {
      destroy_puzzle(p);
      p = new_p;
      printf("s = %d\n", p -> s);

    }

  }

  destroy_puzzle(p);
  
  return 0;
}
