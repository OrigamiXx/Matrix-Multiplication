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
#include "3DM_to_SAT.h"
#ifdef __GUROBI_INSTALLED__
#include "3DM_to_MIP.h"
#endif

#include "heuristic.h"
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
    #ifdef __GUROBI_INSTALLED__
    int s = 1;
    int k = 3;
    int maxs = 4;

    //print_puzzle(p);

    puzzle_row r = 0;
    //p = create_puzzle_from_puzzle(p, r);
    //print_puzzle(p);
    int index = ipow(3,k)-1;
    int vertices[index];

    bool ** graph = (bool **) malloc(sizeof(bool *)* index);
    for (int i = 0; i <= index; i++){
      graph[i] = (bool *)malloc(sizeof(bool)*index);
    }


    // fill in the graph with vertices and edges
    for (int i = 0; i <= index; i++){
      //print_puzzle(p);
      //printf("%s\n", "aaaaaaa");
      for (int j = 0; j <= index; j++){
        puzzle * p  = create_puzzle_from_index(s,k,i);
        r = j;
        p = create_puzzle_from_puzzle(p, r);
        if (check(p) == IS_USP){
          //printf("%s\n", "got here");
          graph[i][j] = true;
          //print_puzzle(p);
          //printf("%s\n", "xxxxxxxxxxxxxxx");
        } else{
          graph[i][j] = false;
          //printf("%s\n", "got here");
        }
        destroy_puzzle(p);
      }
    }
    //printf("%d\n", index);
    // keep track of degree of vertices
    for (int l = 0; l <= index; l++){
        int count = 0;
        for (int a = 0; a <= index; a++){
          if (graph[l][a] == true){
            count++;
          }
        }
        vertices[l] = count;

        //printf("count %d\n", vertices[l]);
    }

    // get rid of the vertices that is less than degree maxs

    int sum;
    for (int b = maxs; b >= 0; b--){
      sum = 0;
      for (int c = 0; c <= index; c++){
        if (vertices[c] <= b){
          for (int d = 0; d <= index; d++){
            graph[c][d] = false;
            graph[d][c] = false;
          }
        }
      }
      for (int e = 0; e <= index; e++){
          int count = 0;
          for (int f = 0; f <= index; f++){
            if (graph[e][f] == true){
              count++;
            }
          }
          vertices[e] = count;
          sum += count;
      }
      if (sum == 0){
        printf("maximum degree is %d\n", b-1);
        break;
      }

    }
    for (int i = 0; i <= index; i++){
      printf("count %d\n", vertices[i]);
    }




    #endif
}
