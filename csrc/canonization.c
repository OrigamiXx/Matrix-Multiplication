/*
 * Module implementing puzzle canonization using Nauty.
 *
 * Author: Matt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <syscall.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "puzzle.h"
#include "nauty.h"
#include "canonization.h"

#define VERT(r,c) ((r) * s + (c) + (s + k))

using namespace std;

void canonize_puzzle(puzzle * p, int s, int k, int n, int * lab, graph * canon_g){
 
  graph g[n * n];
  //graph * canon_g = (graph *)malloc(sizeof(graph) * n * n);//[n * n];
  //graph canon_g[n * n];
  //int lab[n],
  int ptn[n], orbits[n];
  static DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;
  
  int m;
  
  /* Default options are set by the DEFAULTOPTIONS_GRAPH macro above.
     Here we change those options that we want to be different from
     the defaults.  writeautoms=TRUE causes automorphisms to be
     written.  */
  
  options.writeautoms = FALSE;
  options.defaultptn = FALSE;
  options.getcanon = TRUE;

  /* The nauty parameter m is a value such that an array of m setwords
     is sufficient to hold n bits.  The type setword is defined in
     nauty.h.  The number of bits in a setword is WORDSIZE, which is
     16, 32 or 64.  Here we calculate m = ceiling(n/WORDSIZE).  */
  m = SETWORDSNEEDED(n);
  
  /* The following optional call verifies that we are linking
     to compatible versions of the nauty routines.            */  
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
  
  EMPTYGRAPH(g,m,n);

  // Set k-clique in rows.
  for (int r = 0; r < s; r++){
    for (int c1 = 0; c1 < k; c1++){
      for (int c2 = c1 + 1; c2 < k; c2++){
	ADDONEEDGE(g, VERT(r,c1), VERT(r,c2), m);
      }
      ADDONEEDGE(g, VERT(r,c1), r, m);
    }
  }

  // Set s-clique in columns.
  for (int c = 0; c < k; c++){
    for (int r1 = 0; r1 < s; r1++){
      for (int r2 = r1 + 1; r2 < s; r2++){
	ADDONEEDGE(g, VERT(r1,c), VERT(r2,c), m);
      }
      ADDONEEDGE(g, VERT(r1,c), s + c, m);
    }
  }

  // Create label pendants.
  int pendex[3] = {VERT(s,0), VERT(s,0) + 5, VERT(s,0) + 10};

  for (int i = 0; i < 3; i++){
    ADDONEEDGE(g, pendex[i] + 0, pendex[i] + 1, m);
    ADDONEEDGE(g, pendex[i] + 1, pendex[i] + 2, m);
    ADDONEEDGE(g, pendex[i] + 2, pendex[i] + 3, m);
    ADDONEEDGE(g, pendex[i] + 3, pendex[i] + 4, m);
    ADDONEEDGE(g, pendex[i] + 4, pendex[i] + 0, m);
  }

  // Connect puzzles vertices to label pendants.
  for (int r = 0; r < s; r++){
    for (int c = 0; c < k; c++){
      int e = get_entry(p, r, c);
      ADDONEEDGE(g, VERT(r,c), pendex[e-1], m);
    }
  }

  for (int i = 0; i < n; i++){
    lab[i] = i;
    ptn[i] = 1;
    if (i == s - 1 || i == s + k - 1 || i == pendex[0] - 1 || i == n - 1)
      ptn[i] = 0;
  }
  
  //printf("start nauty\n");
  densenauty(g,lab,ptn,orbits,&options,&stats,m,n,canon_g);
  //printf("end nauty\n");
  
  
}

map<string, bool>seen_isomorphs;

// Returns true iff no isomorphs of p have been previously seen.
bool have_seen_isomorph(puzzle * p){

  int s = p -> s;
  int k = p -> k;
  int n = s * k + 3 * (1 + 4) + (s + k);

  int lab[n];
  graph canon_g[n * n];
  
  canonize_puzzle(p, s, k, n, lab, canon_g);

  int m = SETWORDSNEEDED(n);

  size_t graph_size = WORDSIZE / 8 * m * n;

  string key((char *)canon_g, graph_size);

  map<string, bool>::const_iterator iter = seen_isomorphs.find(key);
  if (iter != seen_isomorphs.end()){
    return true;
  }
  
  if (MAX_ISOMORPHS == -1 || seen_isomorphs.size() < MAX_ISOMORPHS)
    seen_isomorphs.insert(pair<string, bool>(key, true));
  
  return false;
}

// Replace p with its canonical isomorph.
void canonize_puzzle(puzzle * p){

  int s = p -> s;
  int k = p -> k;
  int n = s * k + 3 * (1 + 4) + (s + k);

  int lab[n];
  graph canon_g[n * n];
  
  canonize_puzzle(p, s, k, n, lab, canon_g);

  // Create label pendants.
  int pendex[3] = {VERT(s,0), VERT(s,0) + 5, VERT(s,0) + 10};
  
  // Compute permutation of rows / columns and {1,2,3} to canonical graph.
  int row_count = 0;
  int col_count = 0;
  int row_perm[s];
  int col_perm[k];
  int pend_count = 0;
  int pend_perm[3];
  
  for (int i = 0; i < n; i++){

    if (lab[i] < s){
      row_perm[lab[i]] = row_count;
      row_count++;
    } else if (lab[i] < s + k){
      col_perm[lab[i] - s] = col_count;
      col_count++;
    } else if (lab[i] == pendex[0]) {
      pend_perm[0] = pend_count;
      pend_count++;
    } else if (lab[i] == pendex[1]) {
      pend_perm[1] = pend_count;
      pend_count++;
    } else if (lab[i] == pendex[2]) {
      pend_perm[2] = pend_count;
      pend_count++;
    }
    
  }

  puzzle * p_copy = create_puzzle_copy(p);
  for (int r = 0; r < s; r++){
    for (int c = 0; c < k; c++){
      int e = get_entry(p_copy, r, c);
      set_entry(p, row_perm[r], col_perm[c], pend_perm[e - 1] + 1);
    }
  }

  destroy_puzzle(p_copy);

}

