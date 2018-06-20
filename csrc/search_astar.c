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

long h_clique(puzzle * p){
  bool found = false;
  int maxs = 0;
  while (!found){
    int s = 1;
    int k = p->k;
    puzzle_row r = 0;

    puzzle_row max_index = ipow(ipow(3,k),s);
    int vertices[max_index];
    //bool graph[max_index][max_index];

    // Allocate
    bool ** graph = (bool **) malloc(sizeof(bool *) * max_index);
    for (int i = 0; i < max_index; i++){
      graph[i] = (bool *)malloc(sizeof(bool) * max_index);
    }


    // fill in the graph with vertices and edges
    for(puzzle_row i = 0; i < max_index; i++){

      puzzle * p1 = create_puzzle_from_puzzle(p, i);

      for(puzzle_row j = 0; j < max_index; j++){

        puzzle * p2  = create_puzzle_from_index(s, k, j);

        puzzle * p3 = create_puzzle(p->s + 2 * s,k);
        for (int r = 0; r < p->s; r++ ){
          p3->puzzle[r] = p1->puzzle[r];
        }
        for (int r = 0; r < s; r++){
  	       p3->puzzle[p->s+ r] = p1->puzzle[r];
           p3->puzzle[p->s + r+s] = p2->puzzle[r];

        }


        // Insert edge if necessary.
        if (check(p3) == IS_USP)
  	     graph[i][j] = true;
        else
  	     graph[i][j] = false;

        destroy_puzzle(p2);
        destroy_puzzle(p3);

      }

      destroy_puzzle(p1);
    }

    // keep track of degree of vertices
    for (puzzle_row i = 0; i < max_index; i++){

      vertices[i] = 0;

      for (puzzle_row j = 0; j < max_index; j++)
        if (graph[i][j])
  	     vertices[i]++;

      //printf("vertices[%lu] = %d\n",i,vertices[i]);
    }

    // Eliminate vertices that can't be part of maxs-cliques.
    // get rid of the vertices that is less than degree maxs

    // Loop until no further changes are affect in the graph.
    bool changed = true;
    while (changed) {

      changed = false;

      // Delete all vertices with less than maxs degree.
      // Record change, if it occurs.
      for (puzzle_row i = 0; i < max_index; i++){
        if (vertices[i] < maxs){
  	       for (puzzle_row j = 0; j < max_index; j++){
  	          if (graph[i][j] || graph[j][i])
  	           changed = true;
  	            graph[i][j] = false;
  	             graph[j][i] = false;
  	       }
         }
       }

      // Recaulate degrees.
      for (puzzle_row i = 0; i < max_index; i++){

        vertices[i] = 0;

        for (puzzle_row j = 0; j < max_index; j++)
  	     if (graph[i][j])
  	      vertices[i]++;
      }

    }
    int max = vertices[0];
    // Output final degree counts for stable graph.
    for (puzzle_row i = 0; i < max_index; i++){
      if (vertices[i] > max)
        max = vertices[i];
    }
    if (max == 0){
      found = true;
      return maxs;
    } else {
      maxs++;
    }
  }
}

long h(puzzle * p, next_data * prePossible, int n){

  long q = 0;
  for (puzzle_row i=0; i < n; i++){
    puzzle * p1 = create_puzzle_from_puzzle(p, prePossible[i].r);
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
int search(puzzle * p, int prev_best, next_data* prePossible, int n){

  printf("prev_best = %d\n",prev_best);


  int index = p->max_row;
  next_data* possible  = (next_data *) malloc(sizeof(next_data)*index);
  bzero(possible, sizeof(next_data)*index);

  // Ext: Make a copy of prev possible rows
  for (int i = 0; i < n; i++){
    possible[i] = prePossible[i];
  }
  // find out the possible Next
  int a = 0;
  long max_val = 0;
  for (puzzle_row i = 0; i < n; i++){ // Ext: Loop over prev possible rows and update
    puzzle * p1 = create_puzzle_from_puzzle(p, possible[i].r);

    if (check(p1) == IS_USP){
      long value = h_clique(p1); // h(p1,prePossible, n);
      possible[a].r = i;
      possible[a].val = value;
      if (value > max_val)
	     max_val = value;
      a++;
    }
    destroy_puzzle(p1);
  }
  //bzero(prePossible, sizeof(next_data)*index);
  //printf("s = %d, prev_best = %d, max_value = %ld, n = %d\n", p -> s, prev_best, max_val, n);

  int best = prev_best;

  if (a == 0){
    if (p -> s < prev_best)
      return prev_best;
    best = p -> s;
    return best;
  } else if (max_val + 1 + p -> s <= prev_best){
    //printf("term\n");
    return prev_best;
  }

  sort(possible, a);

  for (int i = 0; i < a; i++){
    if ((p->s + 1 + possible[i].val) > best){

      puzzle * p1 = create_puzzle_from_puzzle(p, possible[i].r);

      int result = search(p1, best, possible, a);  // Ext: Pass current possible rows and n.
      if (best < result) {
        best = result;
      }

    } else {
      //free(prePossible);
      free(possible);  // Ext: Free current possible rows
      //printf("best = %d\n",best);
      return best;
    }
  }

  //free(prePossible);
  free(possible);  // Ext: Free current possible rows
  return best;

}

int main(int argc, char ** argv){

  int k = 5;
  puzzle * p = create_puzzle_from_index(1,k,0);
  int index = p->max_row;
  next_data* possible  = (next_data *) malloc(sizeof(next_data)*index);
  bzero(possible, sizeof(next_data)*index);
  for (int i = 0; i<index; i++){
    possible[i].val = 0;
    possible[i].r = i;
  }
  int res = search(p,0,possible, index); // Ext: initialize possible rows before calling.

  printf("res = %d\n", res);

}
