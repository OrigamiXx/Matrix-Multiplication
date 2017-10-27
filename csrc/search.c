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

struct next{
  puzzle_row r;
  long val;
};

// sort the next /in possibleNext in Decreasing order
void sort(struct next* possibleNext, int n){
  int i,j;
  for(i=1;i<n;i++){
        for(j=0;j<n-i;j++){
            if(possibleNext[j].val < possibleNext[j+1].val){
                struct next temp = possibleNext[j];
                possibleNext[j] = possibleNext[j+1];
                possibleNext[j+1] = temp;
            }
        }
    }
}

long h(puzzle * p, puzzle_row r){
  long q = 0;
  for (puzzle_row i=r+1; i < p->max_row; i++){
    puzzle * p1 = create_puzzle(p->s+2, p->k);
    for (int j = 0; i < p->s; j++){
      p1->puzzle[j] = p->puzzle[j];
    }
    p1->puzzle[p->s] = r;
    p1->puzzle[p->s+1] = i;
    if (check(p1) == IS_USP)
      q++;
  }
  return q;
  //return 10000000000;
}


// A* search algorithm
// Input puzzle must be a USP and the row index must be
// sorted in the increasing order(the s th row has the highes row index)
int search(puzzle * p){
  int n = 0;
  int index = p->max_row;
  struct next* possible  = (struct next *) malloc (sizeof(struct next)*index);
  // find out the possible Next
  for (puzzle_row i = p->puzzle[p->s-1]+1; i <= index; i++){
    puzzle * p1 = create_puzzle(p->s+1, p->k);
    for (int j = 0; i < p->s; j++){
      p1->puzzle[j] = p->puzzle[j];
    }
    p1->puzzle[p->s] = i;
    if (check(p1) == IS_USP){
      struct next next1;
      next1.val = h(p, i);
      next1.r = i;
      possible[n] = next1;
      n++;
    }
    destroy_puzzle(p);
  }
  sort(possible, n);
  int best = 0;
  for (int i = 0; i<n; i++){
    if ((p->s + 1 + possible[i].val) > best){
      puzzle * p1 = create_puzzle(p->s+1, p->k);
      for (int j = 0; i < p->s; j++){
        p1->puzzle[j] = p->puzzle[j];
      }
      p1->puzzle[p->s] = i;
      int result = search(p1);
      if (best < result) {
        best = result;
      }
    }else{
      return best;
    }
  }
  return best;

}

int main(int argc, char ** argv){


}
