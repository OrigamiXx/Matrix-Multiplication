/*
Assume a graph G= (V,E)
V = {all the possible rows in a width k size puzzle}
E = {connets two rows that are together is a USP}
We found that (for k = 2) a USP in this graph must be a clique.
We try to write a program to find out all the edges in the graph,
by decreasing the vertices that has not enough desired edges over and over
again, try to find the maximum s that the USP in width k can be.


Not sure this will work.

Author: Jerry
*/


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
#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"

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

int main(int argc, char ** argv){

  int s = 1;
  int k = 4;
  int maxs = 57; // 57 is upper bound for k = 4.

  //puzzle_row r = 0;

  puzzle_row max_index = ipow(ipow(3,k),s);
  int vertices[max_index];
  //bool graph[max_index][max_index];

  // Allocate
  bool ** graph = (bool **) malloc(sizeof(bool *) * max_index);
  for (unsigned int i = 0; i < max_index; i++){
    graph[i] = (bool *)malloc(sizeof(bool) * max_index);
  }


  // fill in the graph with vertices and edges
  for(puzzle_row i = 0; i < max_index; i++){

    puzzle * p1 = create_puzzle_from_index(s, k, i);

    for(puzzle_row j = 0; j < max_index; j++){

      puzzle * p2  = create_puzzle_from_index(s, k, j);

      puzzle * p = create_puzzle(2 * s,k);
      for (int r = 0; r < s; r++){
	       p->puzzle[r] = p1->puzzle[r];
	       p->puzzle[r+s] = p2->puzzle[r];
      }

      // Insert edge if necessary.
      if (check(p) == IS_USP)
	     graph[i][j] = true;
      else
	     graph[i][j] = false;

      destroy_puzzle(p2);
      destroy_puzzle(p);

    }

    destroy_puzzle(p1);
  }

  // keep track of degree of vertices
  for (puzzle_row i = 0; i < max_index; i++){

    vertices[i] = 0;

    for (puzzle_row j = 0; j < max_index; j++)
      if (graph[i][j])
	     vertices[i]++;

    printf("vertices[%lu] = %d\n",i,vertices[i]);
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

  printf("-- Reached fixed point --\n");
  int min = vertices[0];
  int max = vertices[0];
  // Output final degree counts for stable graph.
  for (puzzle_row i = 0; i < max_index; i++){
    if (vertices[i] > max)
      max = vertices[i];
    if ((vertices[i] < min && vertices[i] != 0) || min == 0)
      min = vertices[i];
    printf("vertices[%lu] = %d\n",i,vertices[i]);
  }

  printf("min = %d, max = %d\n", min, max);
  if (max == 0){
    printf("It's not possible to construct a (%d,%d)-USP.\n", maxs, k);
  }

}
