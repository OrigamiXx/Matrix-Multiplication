#include "checker.h"
#include "puzzle.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>

#include "permutation.h"

int ipow(int base, int exp){
  int result = 1;
  while (exp) {
    if (exp & 1){
      result *= base;
    }
    exp >>=1;
    base *=base;
  }
  return result;
}

typedef struct next{
  puzzle_row r;
  long val;
} next_data;

// sort the next /in possibleNext in Decreasing order
void sort(next_data* possibleNext, int n){
  int i, j;
  for(i = 1; i < n; i++){
        for(j = 0; j < n - i; j++){
            if(possibleNext[j].val < possibleNext[j+1].val){
                next_data temp = possibleNext[j];
                possibleNext[j] = possibleNext[j+1];
                possibleNext[j+1] = temp;
            }
        }
    }
}

long h_inf(puzzle * p){

  return 100000000000L;
}

long h(puzzle * p){ // Ext: Pass previous rows
  
  long q = 0;
  for (puzzle_row i=0; i < p->max_row; i++){ // Ext: Loop over possible rows
    puzzle * p1 = create_puzzle_from_puzzle(p, i);
    if (check(p1) == IS_USP){
      q++;
    }
    destroy_puzzle(p1);
  }
  return q;
}


// A* search algorithm
// Input puzzle must be a USP and the row index must be
// sorted in the increasing order(the s th row has the highes row index)
int search(puzzle * p, int prev_best){  // Ext: Pass previous possible rows and n

  printf("prev_best = %d\n",prev_best);
  
  int n = 0;
  int index = p->max_row;
  next_data* possible  = (next_data *) malloc(sizeof(next_data)*index);  // Ext: Make a copy of prev possible rows
  bzero(possible, sizeof(next_data)*index);
  // find out the possible Next

  long max_val = 0;
  for (puzzle_row i = 0; i <= index; i++){ // Ext: Loop over prev possible rows and update
    puzzle * p1 = create_puzzle_from_puzzle(p, i);

    if (check(p1) == IS_USP){
      long value = h_inf(p1); // Ext: Pass possible rows.
      possible[n].r = i;
      possible[n].val = value;
      if (value > max_val)
	max_val = value;
      
      n++;
    }

    
    destroy_puzzle(p1);
  }

  //printf("s = %d, prev_best = %d, max_value = %ld, n = %d\n", p -> s, prev_best, max_val, n);

  int best = prev_best;
  
  if (n == 0){
    if (p -> s < prev_best)
      return prev_best;
    best = p -> s;
    return best;
  } else if (max_val + 1 + p -> s <= prev_best){
    //printf("term\n");
    return prev_best;
  }

  sort(possible, n);
  
  for (int i = 0; i < n; i++){
    if ((p->s + 1 + possible[i].val) > best){
      
      puzzle * p1 = create_puzzle_from_puzzle(p, possible[i].r);

      int result = search(p1, best);  // Ext: Pass current possible rows and n.
      if (best < result) {
        best = result;
      }
      
    } else {
      
      free(possible);  // Ext: Free current possible rows
      //printf("best = %d\n",best);
      return best;
    }
  }


  free(possible);  // Ext: Free current possible rows
  return best;

}

int main(int argc, char ** argv){
  
  puzzle * p = create_puzzle_from_index(1,7,0);

  int res = search(p,0); // Ext: initialize possible rows before calling.

  printf("res = %d\n", res);
  
}
