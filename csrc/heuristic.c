/*
 * Module implementing several non-trivial heursitics for checking
 * whether a given puzzle is a strong USP.
 * 
 * Heuristics MUST match the checker_t type and semantics, and should
 * be named with a prefix "heuristic_".
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

#include "matching.h"
#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"
#include "checker.h"
#include "puzzle.h"
#include "nauty.h"

/*
 * A specialized function that determines whether any pair of rows
 * prevent an s-by-k from being a strong USP.  Returns NOT_USP or
 * UNDET_USP.
 */
check_t heuristic_row_pairs(puzzle * p){
  int s = p -> s;
  for (int r1 = 0; r1 < s - 1; r1++){
    for (int r2 = r1 + 1; r2 < s; r2++){
      check_t res = check2(p -> puzzle[r1], p -> puzzle[r2], p -> k);
      if (res == NOT_USP)
	return NOT_USP;
    }
  }
  return UNDET_USP;
}

/*
 * A specialized function that determines whether any triple of rows
 * prevent an s-by-k from being a strong USP.  Return NOT_USP or
 * UNDET_USP.
 */
check_t heuristic_row_triples(puzzle * p){

  int s = p -> s;
  for (int r1 = 0; r1 <  s - 2; r1++){
    for (int r2 = r1 + 1; r2 <  s - 1; r2++){
      for (int r3 = r2 + 1; r3 < s; r3++){
	check_t res = check3(p -> puzzle[r1], p -> puzzle[r2], p -> puzzle[r3], p -> k);
	if (res == NOT_USP)
	  return NOT_USP;
      }
    }
  }
  return UNDET_USP;
}

/*
 * Polytime heuristic that checks whether 2DM exists for each of the
 * projected faces of the 3DM instance.  Returns UNDET_USP or IS_USP.
 * XXX - Almost never returns IS_USP, may be broken.
 */
check_t heuristic_2d_matching(puzzle * p){

  invalidate_tdm(p);
  //compute_tdm(p);
  simplify_tdm(p);
  
  int s = p -> s;
  bool M[s * s];
  bool found = false;
  
  for (int del = 0; del < s && !found; del++){

    set_tdm_entry(p, del, del, del, false);
    
    for (int i = 0; i < s; i++){
      for (int j = 0; j < s; j++){
	M[i * s + j] = false;
	for (int k = 0; k < s; k++){
	  M[i * s + j] = M[i * s + j] || get_tdm_entry(p, i, j, k);
	}
      }
    }

    bool found1 = has_perfect_bipartite_matching(M, s);
  
    for (int i = 0; i < s; i++){
      for (int j = 0; j < s; j++){
	M[i * s + j] = false;
	for (int k = 0; k < s; k++){
	  M[i * s + j] = M[i * s + j] || get_tdm_entry(p, k, i, j);
	}
      }
    }
    
    bool found2 = has_perfect_bipartite_matching(M, s);

    for (int i = 0; i < s; i++){
      for (int j = 0; j < s; j++){
	M[i * s + j] = false;
	for (int k = 0; k < s; k++){
	  M[i * s + j] = M[i * s + j] || get_tdm_entry(p, j, k, i);
	}
      }
    }

    bool found3 = has_perfect_bipartite_matching(M, s);
    found = found || (found1 && found2 && found3);
    
    set_tdm_entry(p, del, del, del, true);

  }

  if (found)
    return UNDET_USP;
  else 
    return IS_USP;
}



/*
 *  Reorders rows in an attempt to make fewer entries be generated.
 *  They can be arranged in increasing or decreasing order, or outside
 *  -> inside, or inside -> outside.
 *  XXX - Make it also permute the puzzle?
 */
void reorder_witnesses(puzzle * p, bool increasing, bool sorted){

  int s = p -> s;
  bool * tdm = p -> tdm;
  
  // Count the witnesses in each layer.
  int layer_counts[s];
  for (int i = 0; i < s; i++){
    layer_counts[i] = 0;
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	if (get_tdm_entry(p, i, j, r))
	  layer_counts[i]++;
      }
    }
    //printf("layer_count[%d] = %d\n", i, layer_counts[i]);
  }

  // Controls whether order is increasing or decreasing.
  int increase = (increasing ? 1 : -1);

  // Compute a permutate that orders layers as desired.
  int perm[s];
  bool tdm2[s * s * s];
  for (int i = 0; i < s; i++){
    int opt_index = -1;
    int opt = increase * (s*s + 1);

    for (int j = 0; j < s; j++){
      if (increase * layer_counts[j] < increase * opt) {
	opt = layer_counts[j];
	opt_index = j;
      }
    }

    int to_index = i;

    if (sorted) {
      // Layers will appear in a sorted order.
      to_index = i;
    } else {
      // Layers will appear order from outside to inside.
      if (i % 2 == 0)
	to_index = (int)(i / 2.0);
      else
	to_index = s - (int) ((i + 1) / 2.0);
    }

    //printf("to_index = %d, from_index = %d\n", to_index, minIndex);

    layer_counts[opt_index] = increase * (s * s + 2);
    perm[opt_index] = to_index;

  }

  // Reorder the layers in a new matrix.
  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	int ix = perm[i] * s * s + perm[j] * s + perm[r];
	tdm2[ix] = get_tdm_entry(p, i, j, r);
      }
    }
  }

  /*
  // Check that layers are arranged.
  for (int i = 0; i < s; i++){
    layer_counts[i] = 0;
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	if (tdm2[i * s * s + j * s + r])
	  layer_counts[i]++;
      }
      printf("layer_count[%d] = %d\n", i, layer_counts[i]);
    }
  }
  */

  // Copy back to original matrix.
  memcpy(tdm, tdm2, s * s * s * sizeof(bool));
  
}


/*
 * Returns NOT_USP iff it finds a witness that puzzle is not a strong USP.
 * UNDET_USP indicates the search was inconclusive.  Requires s <= 31.
 */
check_t heuristic_random_witness(puzzle * p, int repeats){

  int s = p -> s;
  bool * tdm = p -> tdm;
  
  if (s > 31)
    return UNDET_USP;

  bool failed = false;
  int best = 0;
  int total = 0;
  for (int n = 0; n < repeats && !failed; n++){

    set_long curr = CREATE_EMPTY();
    bool is_ident = true;
    for (int i = 0 ; i < s && !failed; i++){

      int offset_j = lrand48() % s;
      int offset_k = lrand48() % s;
      bool found = false;

      for (int j = 0 ; j < s && !found; j++){
	int shift_j = (j + offset_j) % s;
	if (MEMBER_S2(curr, shift_j))
	  continue;
	for (int k = 0; k < s && !found; k++){
	  int shift_k = (k + offset_k) % s;
	  if (MEMBER_S3(curr, shift_k))
	    continue;

	  if (tdm[i * s * s + shift_j * s + shift_k]) {
	    if (shift_k != shift_j || shift_k != i)
	      is_ident = false;
	    curr = INSERT_S2(INSERT_S3(curr, shift_k), shift_j);
	    found = true;
	  }
	}
      }

      if (!found) {
	best = (best < i ? i : best);
	total += i;
	failed = true;
	//printf("Failed to locate witness in random. Acheived: %d / %d, id: %d\n", i, s,is_ident);
      }
    }
    if (!failed && !is_ident)
      return NOT_USP;

    // Not sure if this is a good heuristic stopping condition, it is
    // to prevent a lot of time being waste on small puzzle sizes.  It
    // makes longer puzzles slower.  XXX - Improve parameters.
    if ((log(n + 1) / log(2))  > best)
      return UNDET_USP;
    failed = false;
  }

  return UNDET_USP;
}


int default_heuristic_iteration(int s){

  return s * s * s;
}


/*
 * Returns NOT_USP iff it finds a witness that puzzle is not a strong
 * USP.  UNDET_USP indicates the search was inconclusive.  Attempts to
 * generate a witness by greedily selecting a layer from among those
 * with fewest remaining edges and then uniformly selects an edge from
 * that layer.  This repeats until a witness is found, or no progress
 * can be made.  This process repeats for some specified number of
 * iterations.  There is no benefit to reorder_witnesses() be called
 * before this.  Requires s <= 31.
 */
check_t heuristic_greedy(puzzle * p, int repeats){

  int s = p -> s;
  simplify_tdm(p);
  bool * tdm = p -> tdm;
  
  if (s > 31)
    return UNDET_USP;

  bool failed = false;
  int best_depth = 0;
  int total = 0;
  for (int n = 0; n < repeats && !failed; n++){

    set_long curr_set = CREATE_EMPTY();

    bool is_ident = true;

    int layer_counts[s];

    for (int i = 0 ; i < s ; i++){
      layer_counts[i] = 0;
      for (int j = 0 ; j < s ; j++){
	for (int k = 0 ; k < s ; k++){
	  if (tdm[i * s * s + j * s + k])
	    layer_counts[i]++;
	}
      }
      //printf("layer_counts[%d] = %d\n",i, layer_counts[i]);
    }
    int too_big = s * s + 1;

    for (int i = 0 ; i < s ; i++){

      // Randomly select a layer with the least choices.
      int best = too_big;
      int num_best = 0;
      int layer = 0;
      for (layer = 0 ; layer < s ; layer++){
	if (layer_counts[layer] < best) {
	  best = layer_counts[layer];
	  num_best = 1;
	} else if (layer_counts[layer] == best){
	  num_best++;
	}
      }

      if (best == 0) {
	// There is an empty layer.
	failed = true;
	best_depth = (best_depth < i ? i : best_depth);
	total += i;
	break;
      }

      assert(best < too_big);
      assert(num_best >= 1);
      int choice = lrand48() % num_best;
      int found = -1;
      for (layer = 0; layer < s && found < choice ; layer++){
	if (layer_counts[layer] == best)
	  found++;
      }
      // Correct for final increment.
      layer--;  // This the layer to select something in.

      // Randomly select a choice from this layer uniformly at random.
      found = -1;
      choice = lrand48() % layer_counts[layer];
      //printf("choice = %d\n", choice);

      int j = 0;
      int k = 0;
      for (j = 0 ; j < s && found < choice ; j++){
	if (MEMBER_S2(curr_set, j)) continue;
	for (k = 0; k < s && found < choice ; k++){
	  if (MEMBER_S3(curr_set, k)) continue;

	  if (tdm[layer * s * s + j * s + k]){
	    found++;
	    //printf("found!\n");
	    if (found == choice) {
	      layer_counts[layer] = too_big; // Means layer is already processed.
	      curr_set = INSERT_S2(curr_set, j);
	      curr_set = INSERT_S3(curr_set, k);
	      if (layer != j || layer != k)
		is_ident = false;
	    }
	  }
	}
      }
      // Correct for final increment.
      j--;
      k--;

      //printf("(j, k) = (%d,%d)\n", j, k);

      //printf("i = %d, layer = %d  %d\n", i, layer, layer_counts[layer]);
      assert(layer_counts[layer] == too_big);
      // Update layer_counts.
      for (int layer2 = 0 ; layer2 < s ; layer2++){
	if (layer_counts[layer2] != too_big) {
	  for (int j2 = 0; j2 < s; j2++){
	    if (!MEMBER_S2(curr_set, j2) && tdm[layer2 * s * s + j2 * s + k])
	      layer_counts[layer2]--;
	  }
	  for (int k2 = 0; k2 < s; k2++){
	    if (!MEMBER_S3(curr_set, k2) && tdm[layer2 * s * s + j * s + k2])
	      layer_counts[layer2]--;
	  }
	  if (tdm[layer2 * s * s + j * s + k])
	    layer_counts[layer2]--;
	  assert(layer_counts[layer2] >= 0);
	}
	//printf("layer_counts[%d] = %d\n",layer2, layer_counts[layer2]);
      }
      //printf("\n");
    }

    /*
    int layer_counts_chk[s];
    for (int ix = 0 ; ix < s ; ix++){
      layer_counts_chk[ix] = 0;
      for (int j = 0 ; j < s ; j++){
	if (MEMBER_S2(curr_set, j)) continue;
	for (int k = 0 ; k < s ; k++){
	  if (MEMBER_S3(curr_set, k)) continue;
	  if (tdm[ix * s * s + j * s + k])
	    layer_counts_chk[ix]++;
	}
      }
      //printf("layer_counts_chk[%d] = %d\n",ix, layer_counts_chk[ix]);
      assert(layer_counts[ix] == layer_counts_chk[ix] || layer_counts[ix] == too_big);
    }
    */

    if (!failed && !is_ident)
      return NOT_USP;

    // Not sure if this is a good heuristic stopping condition, it is
    // to prevent a lot of time being waste on small puzzle sizes.  It
    // makes longer puzzles slower.  XXX - Improve parameters.
    //if ((log(n + 1) / log(2)) > best_depth)
    //  return false;
    failed = false;
  }

  invalidate_tdm(p);
  
  return UNDET_USP;
}

check_t heuristic_greedy(puzzle * p){

  if (p -> s > 31)
    return UNDET_USP;
  
  return heuristic_greedy(p, default_heuristic_iteration(p -> s));
  
}


// Some variables measuring performance.
int size_forward = 0;
int last_layer_forward = 0;
int size_backward = 0;
int checks_backward = 0;


/*
 * Heuristically precheck puzzle via random and greedy approaches
 * Returns IS_USP if puzzle is a strong USP.  Returns NOT_USP if
 * puzzle is not a strong USP.  Returns UNDET_USP if the function has
 * not determined the puzzle is a strong USP.
 */
check_t heuristic_random(puzzle * p, int iter){

  // Rearrange row_witness in the hope it makes the search faster.
  // Then randomly attempt to build a witness that U is not a strong
  // USP.  The number of iterations is a bit ad hoc; s*s*s also seeme
  // to work well in the domain of parameters I profiled.  XXX -
  // Improve parameters.  Doing it twice for two different ordering of
  // the puzzle was the most effective.

  // This reorder is aimed to make the randomize search more likely to
  // succeed.
  simplify_tdm(p);
  check_t res = UNDET_USP;
  
  reorder_witnesses(p, true, true);
  if (heuristic_random_witness(p, iter) == NOT_USP){
    res = NOT_USP;
  } else {
    
    reorder_witnesses(p, false, false);
    if (heuristic_random_witness(p, iter) == NOT_USP){
      res = NOT_USP;
    } else {

      // This reorder is aimed to make the forward and backward search
      // balanced.
      reorder_witnesses(p, true, false);
      if (heuristic_random_witness(p, iter) == NOT_USP){
	res = NOT_USP;
      }
    }
  }

  invalidate_tdm(p);
  
  return res;
}

check_t heuristic_random(puzzle * p) {

  if (p -> s > 31)
    return UNDET_USP;

  return heuristic_random(p, default_heuristic_iteration(p -> s));
  
}

check_t heuristic_graph_automorphism(puzzle * p){

  int n = 3 * p -> s + p -> k;
  
  graph g[n*n];
  int lab[n],ptn[n],orbits[n];
  static DEFAULTOPTIONS_GRAPH(options);
  statsblk stats;
  
  int m;
  
  /* Default options are set by the DEFAULTOPTIONS_GRAPH macro above.
     Here we change those options that we want to be different from
     the defaults.  writeautoms=TRUE causes automorphisms to be
     written.  */
  
  options.writeautoms = FALSE;
  options.defaultptn = FALSE;
  

  
  /* The nauty parameter m is a value such that an array of m setwords
     is sufficient to hold n bits.  The type setword is defined in
     nauty.h.  The number of bits in a setword is WORDSIZE, which is
     16, 32 or 64.  Here we calculate m = ceiling(n/WORDSIZE).  */
    
  m = SETWORDSNEEDED(n);
  
  /* The following optional call verifies that we are linking
     to compatible versions of the nauty routines.            */
  
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
  
  /* Now we create the cycle.  First we zero the graph, than for
     each v, we add the edge (v,v+1), where values are mod n. */
  
  EMPTYGRAPH(g,m,n);
  
  for (int r = 0; r < p -> s; r++){
    for (int c = 0; c < p -> k; c++){
      int entry = get_entry(p, r, c);
      int a = 3 * r + (entry - 1);
      int b = 3 * p -> s + c;
      //printf("adding edge %d -> %d\n",a,b);
      ADDONEEDGE(g, a, b, m);
    }
    ptn[3*r] = 1;
    ptn[3*r+1] = 2;
    ptn[3*r+2] = 3;
  }
  
  for (int i = 0; i < 3; i++){
    for (int r = 0; r < p -> s; r++){
      lab[p -> s * i + r] = 3 * r + i;
      ptn[p -> s * i + r] = (r == p -> s - 1 ? 0 : 1);
    }
  }
  
  for (int c = 0; c < p -> k; c++){
    lab[3 * p -> s + c] = 3 * p -> s + c;
    ptn[3 * p -> s + c] = (c == p -> k - 1 ? 0 : 1);
  }
  
  /*
    for (int i = 0; i < n; i++){
    printf("lab, ptn[%d] = %d, %d\n",i,lab[i], ptn[i]);
    }
  */
  
  
  densenauty(g,lab,ptn,orbits,&options,&stats,m,n,NULL);
  
  //printf("%d, %f, %d\n", stats.errstatus, stats.grpsize1, stats.grpsize2);
  if (stats.numgenerators == 0) {
    
    //printf("Rigid\n");
    return UNDET_USP;
    //return IS_USP;
    
  } else {
    
    //printf("Not rigid: %d generators\n", stats.numgenerators);
    
    return NOT_USP;
  }
  
}
