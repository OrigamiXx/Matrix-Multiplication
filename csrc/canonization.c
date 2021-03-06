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

#define VERT(r,c) ((r) * k + (c) + (s + k))

using namespace std;

void canonize_puzzle(puzzle * p, int s, int k, int n, int * lab, graph * canon_g){

  /* The nauty parameter m is a value such that an array of m setwords
     is sufficient to hold n bits.  The type setword is defined in
     nauty.h.  The number of bits in a setword is WORDSIZE, which is
     16, 32 or 64.  Here we calculate m = ceiling(n/WORDSIZE).  */
  int m = SETWORDSNEEDED(n);

  graph g[m * n];
  int ptn[n], orbits[n];
  static DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;

  /* Default options are set by the DEFAULTOPTIONS_GRAPH macro above.
     Here we change those options that we want to be different from
     the defaults.  writeautoms=TRUE causes automorphisms to be
     written.  */

  options.writeautoms = FALSE;
  options.defaultptn = FALSE;
  options.getcanon = TRUE;

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

  /*
  for (int i = 0; i < n; i++){
    printf("%d: \t %016lx\n", i, g[i]);

  }
  */

  densenauty(g,lab,ptn,orbits,&options,&stats,m,n,canon_g);

}

bool initilized = false;
map<string, int> *seen_isomorphs;
int num_seen = 0;

map<string, int> *stored_isomorphs;  //Fix this use pointers
int stored_seen = 0;

void ensure_initilize()
{
  if(!initilized)
  {
    seen_isomorphs = new map<string, int>();
    stored_isomorphs = new map<string, int>();
    initilized = true;
  }
}


// Returns true iff no isomorphs of p have been previously seen.
bool have_seen_isomorph(puzzle * p, bool remember, int * index){

  ensure_initilize();
  int s = p -> s;
  int k = p -> k;
  int n = s * k + 3 * (1 + 4) + (s + k);

  int lab[n];
  int m = SETWORDSNEEDED(n);
  size_t graph_size = sizeof(graph) * m * n;
  graph canon_g[m * n];
  bzero(canon_g, graph_size);

  canonize_puzzle(p, s, k, n, lab, canon_g);

  string key((char *)canon_g, graph_size);

  map<string, int>::const_iterator iter = seen_isomorphs -> find(key);
  if (iter != seen_isomorphs -> end()){
    if (index != NULL)
      *index = iter->second;
    return true;
  }

  if (remember && (MAX_ISOMORPHS == -1 || seen_isomorphs -> size() < MAX_ISOMORPHS)){
    num_seen++;
    if (index != NULL)
      *index = num_seen;
    seen_isomorphs -> insert(pair<string, int>(key, num_seen));
  }

  return false;
}

// Reset the set of previously seen isomorphs.
void reset_isomorphs(){

  ensure_initilize();
  seen_isomorphs -> erase(seen_isomorphs -> begin(), seen_isomorphs ->end());
  assert(seen_isomorphs->size() == 0);
  num_seen = 0;
}

void swap_stored_state()
{
  ensure_initilize();
  map<string, int>* temp = seen_isomorphs;
  seen_isomorphs = stored_isomorphs;
  stored_isomorphs = temp;

  int temp_seen = num_seen;
  num_seen = stored_seen;
  stored_seen = temp_seen;
}

// Replace p with its canonical isomorph.
void canonize_puzzle(puzzle * p){

  int s = p -> s;
  int k = p -> k;
  int n = s * k + 3 * (1 + 4) + (s + k);

  int lab[n];
  int m = SETWORDSNEEDED(n);
  graph canon_g[m * n];

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

// Replace p with its canonical isomorph.
bool are_isomorphs(puzzle * p1, puzzle * p2){

  assert(p1 -> k == p2 -> k);
  assert(p1 -> s == p2 -> s);

  int s = p1 -> s;
  int k = p1 -> k;
  int n = s * k + 3 * (1 + 4) + (s + k);

  int lab[n];
  int m = SETWORDSNEEDED(n);
  graph canon_g1[m * n];
  graph canon_g2[m * n];

  canonize_puzzle(p1, s, k, n, lab, canon_g1);
  canonize_puzzle(p2, s, k, n, lab, canon_g2);

  for (int i = 0 ; i < m * n; i++)
    if (canon_g1[i] != canon_g2[i])
      return false;

  return true;

}

size_t get_num_isomorphs(){
  ensure_initilize();
  return seen_isomorphs -> size();
}
