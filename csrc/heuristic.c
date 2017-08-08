#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <set>
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
#include "usp.h"
#include "puzzle.h"


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
 * XXX - Almost never returns IS_USP.
 */
check_t heuristic_2d_matching(puzzle * p){

  int s = p -> s;
  bool M[s * s];

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || get_tdm_entry(p, i, j, k);
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return IS_USP;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || get_tdm_entry(p, k, i, j);
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return IS_USP;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || get_tdm_entry(p, j, k, i);
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return IS_USP;

  return UNDET_USP;
}



/*
 *  Reorders rows in an attempt to make fewer entries be generated.
 *  They can be arranged in increasing or decreasing order, or outside
 *  -> inside, or inside -> outside.
 *  XXX - Make it also permute the puzzle?
 */
void reorder_witnesses(puzzle * p, bool increasing, bool sorted){

  int s = p -> s;
  compute_tdm(p);
  bool * tdm = p -> tdm;
  
  // Count the witnesses in each layer.
  int layer_counts[s];
  for (int i = 0; i < s; i++){
    layer_counts[i] = 0;
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	if (tdm[i * s * s + j * s + r])
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
	tdm2[ix] = tdm[i * s * s + j * s + r];
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
check_t has_random_witness(puzzle * p, int repeats){

  int s = p -> s;
  compute_tdm(p);
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
      return NOT_USP;
    failed = false;
  }

  return NOT_USP;
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
  compute_tdm(p);
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

  return UNDET_USP;
}

check_t heuristic_greedy(puzzle * p){

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
  reorder_witnesses(p, true, true);
  if (has_random_witness(p, iter)){
    return NOT_USP;
  }
    
  reorder_witnesses(p, false, false);
  if (has_random_witness(p, iter)){
    return NOT_USP;
  }

  // This reorder is aimed to make the forward and backward search
  // balanced.
  reorder_witnesses(p, true, false);
  if (has_random_witness(p, iter)){
    return NOT_USP;
  }

  return UNDET_USP;
}

check_t heuristic_random(puzzle * p) {

  return heuristic_random(p, default_heuristic_iteration(p -> s));
  
}



// Experimental application of simplify_tdm on usp_bi.
check_t heuristic_simplify(puzzle * p){

  simplify_tdm(p);
  return check_usp_bi(p);

}
