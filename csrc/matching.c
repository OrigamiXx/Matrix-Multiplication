#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "matching.h"

// Iterative depth-first search for augmenting path.  O(n^2)
bool augment_path(bool *G, int s, int t, int n){

  int prev[n];
  for (int i = 0 ; i < n ; i++){
    prev[i] = -1;
  }

  prev[s] = s;

  int curr = s;
  int next = 0;

  while (curr != s || next != n){

    //    printf("n = %d, curr = %d, next = %d\n", n, curr, next);
    
    for ( ; next < n; next++){
      //printf("G[%d * n + %d] = %d\n",curr, next, G[curr * n + next]);
      if (curr != next && G[curr * n + next] && prev[next] == -1){
	//printf("Found edge (%d, %d)\n", curr, next);
	if (next == t) {
	  // Loop and update augmenting path.
	  //printf("Found destination\n");
	  while (next != s) {
	    G[curr * n + next] = false;
	    G[next * n + curr] = true;
	    next = curr;
	    curr = prev[curr];
	  }
	  return true;
	}
	prev[next] = curr;
	curr = next;
	next = 0;
	break;
      }
    }
    
    if (next == n  && curr != s) {
      next = curr + 1;
      curr = prev[curr];
    }
  }
  
  
  return false;
}

void print_G(bool *G, int n){

  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (G[i * n + j])
	printf("1");
      else
	printf("0");
    }
    printf("\n");
  }

}

// Implement Bipartite matchin using Ford-Faulkerson flow algorithm.
// O(n^3) implementation.
bool has_perfect_bipartite_matching(bool *M, int n){

  int N = 2 * n + 2;
  bool G[N * N];
  int s = N - 2;
  int t = N - 1;
  
  // 1. Initialize data structures
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      G[i * N + j] = false;
  
  for (int i = 0 ; i < n ; i++){
    G[s * N + i] = true;
    G[(n + i) * N + t] = true;
    for (int j = 0 ; j < n ; j++){
      G[i * N + (n + j)] = M[i * n + j];
    }
  }

  //print_G(G, N);
  
  // 2. Locate augmenting path.
  // 3. If path exists, put flow on path and repeat from Step 2.
  int matching_size = 0;
  while(augment_path(G, s, t, N)) {
    //print_G(G, N);
    matching_size++;
  }
    

  //printf("maximum matching = %d\n", matching_size);
  // 4. No path exists, return
  return matching_size == n;
}
