/*
 * Module implementing several non-trivial methods for checking
 * whether a given puzzle is a strong USP.
 *
 * 1. Undirectional search for a witness that puzzle is not a strong USP.
 * 2. Bidirectional search for a witness that puzzle is not a strong USP.
 * 3. Hybrid search using 1 & 2 with the option to precompute and
 *    cache small puzzles.
 *
 * Uses it's own representation of sets and permutations.
 *
 * Author: Matt & Jerry.
 */

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

using namespace std;


//give a size of the puzzle check all the puzzles from 1X1 to this size
// whether they are USPs and
void check_all_usp(int s, int k){
  int res_counts[2][2] = {0};

  for(int i = 1; i <= s; i++){
    for(int j = 1; j <= k; j++){
      
      int max_row = MAX_ROWS[j];

      for(int row = 0; row < max_row; row++){

	puzzle * p = create_puzzle_from_index(i, j, row);
	bool res_1 = check_usp_bi(p);//check_usp(p);
	bool res_2 = check_usp_uni(p); //  check_usp_mult(p);

	res_counts[res_1][res_2]++;

	destroy_puzzle(p);
      }
      
      printf("s = %d k = %d TT = %d FF = %d TF = %d FT = %d\n", i, j,
	     res_counts[1][1], res_counts[0][0], res_counts[1][0], res_counts[0][1]);
      assert(res_counts[1][0] == 0 && res_counts[0][1] == 0);

    }
  }

}

//check the number of the usp of puzzles with same width and different height
/*
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if GCC_VERSION > 40400

int check_usp_same_col(int max_row, int column){
  std::map<string, puzzle*>M;
  int i, max_poss_row=1, row = 1;
  for(i = 0; i<column; i++){
    max_poss_row = max_poss_row*3;
  }
  int tt = 0;
  puzzle * p;
  for(i = 0; i<max_poss_row; i++){
    p = create_puzzle_from_index(row, column, i);
    M.insert(pair<string,  puzzle*>(to_string(i), p));
    tt++;
  }
  //for(map<string, puzzle*>::iterator it = M.begin(); it!=M.end(); ++it){

  //}
  printf("row %d column %d has %d usps.\n",row,column,tt);
  for(row = 2; row <= max_row; row++){
    map<string, puzzle*>tmpM;
    tt = 0;
    int total = 0;
    for(map<string, puzzle*>::iterator it = M.begin(); it!=M.end(); ++it){
      int t = 0;
      for(i = it->second->puzzle[it->second->row-1]; i<max_poss_row; i++){
	p = create_puzzle_from_puzzle(it->second, i);
	total++;
	if(check_usp_recursive(p)){
	  tmpM.insert(pair<string, puzzle*>(it->first+"_"+ to_string(i), p));
	  t++;
	  tt++;
	}

      }
      destroy_puzzle(it->second);
      if(total%500 == 0){
	cout << "\r"<<it->first;
	printf(" has %d usps on the when adds one more row to it.", t);
      }
    }
    M.clear();
    M = tmpM;
    printf("column %d, row %d has %d usps.\n", column, row, tt);
  }
  return 0;
}

#endif
*/


/*
 *=============================================================
 *
 *  Helper Functions
 *
 *=============================================================
 */


/*
 * Checks whether the given vector pi represents the identity
 * permutation of length s.  Runtime is O(s).
 */
bool ident(vector<int> pi, int s){
  for (int i = 0; i < s; i++){
    if (pi[i] != i) return false;
  }
  return true;
}

/*
 * Polytime checks for row_witness properties that decide whether U is
 * a strong USP.  Return 1, if U must be a strong USP, -1 if U cannot
 * be a strong USP, and 0 if it is not determined.  Doesn't appear to
 * be trigger on the examples I tested.
*/
int precheck_row_witness(bool * row_witness, int s){

  bool M[s * s];

  int false_count = 0;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || !row_witness[i * s * s + j * s + k];
	if (!row_witness[i * s * s + j * s + k])
	  false_count++;
      }
    }
  }

  //printf("Density = %f\n", ((float)false_count)/(s * s * s));

  if (!has_perfect_bipartite_matching(M, s))
    return 1;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || !row_witness[k * s * s + i * s + j];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return 1;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int k = 0; k < s; k++){
	M[i * s + j] = M[i * s + j] || !row_witness[j * s * s + k * s + i];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return 1;

  return 0;
}


/*
 *=============================================================
 *
 *  Checking for Strong USPs
 *
 *=============================================================
 *
 * A puzzle U is a set of length-k strings over {1,2,3}.  We say that
 * U is a strong uniquely solvable puzzle (USP) if for all pi_1, pi_2,
 * pi_3 in Symmetric(U), either pi_1 = pi_2 = pi_3 or there exists u
 * in U and i in [k] such that exactly two of (pi_1(u))_i = 1,
 * (pi_2(u))_i = 2, and (pi_3(u))_i = 3 hold.
 *
 * While checking this condition One can assume without loss of
 * generality that pi_1 is the identity permutation, because only the
 * relative difference in the permutations is relevant.  In
 * particular, if the inner condition holds (or doesn't) for pi_1,
 * pi_2, pi_3, u, and i, it also holds for 1, pi_1^-1 o pi_2, pi_1^-1 o
 * pi_3, pi_1^-1(u), and i.
 */

/*
 *-------------------------------------------------------------
 *  Unidirectional Test
 *-------------------------------------------------------------
 *
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a unidirectional algorithm that tests all permutations pi_2 and
 * pi_3 to see whether they witness that U is not a strong USP.
 *
 * This is an alternative implementation of check_usp from usp.c.  It
 * uses c++ iterators with the built-in function next_permutation to
 * loop over all permutations.
 */
check_t check_usp_uni(puzzle * p){

  int s = p -> s;
  
  // Precompute s * s * s memoization table storing the mapping of
  // rows that witness that a partial mapping is consistent with U
  // being a strong USP.  For U to not be a strong USP there must a
  // way to select s false entries from the table whose indexes are
  // row, column, and slice disjoint.
  compute_tdm(p);

  // Create two vectors for storing permutations over the set of s
  // rows.  pi_1 is not explicitly represented, because it will always
  // be the identity.
  vector<int> pi_2(s);
  vector<int> pi_3(s);

  // We look for pi_2 and pi_3 that witness that U is not a strong
  // USP.

  // Initialize pi_2 to the identity permutation.
  for (int i = 0; i < s; i++) pi_2[i] = i;

  // Loop over all permutations pi_2.
  bool go1 = true;
  for (; go1 ; go1 = next_permutation(pi_2.begin(),pi_2.end())){

    // Initialize pi_3 to the identity permutation.
    for (int i = 0; i < s; i++) pi_3[i] = i;
    int go2 = true;

    // Loop over all permutations pi_3.
    for (; go2 ; go2 = next_permutation(pi_3.begin(),pi_3.end())){

      // Skip if pi_1, pi_2, pi_3 are the same -- all identity.
      if (ident(pi_2,s) && ident(pi_3,s)) continue;

      // Check whether pi_1, pi_2, pi_3 touch only trues.
      bool found = false;
      for (int i = 0; i < s; i++){
	if (!(p -> tdm[i * s * s + pi_2[i] * s + pi_3[i]])){
	  found = true;
	  break;
	}
      }

      if (!found)
	// pi_2 and pi_3 are a witness that U is not a strong USP.
	return NOT_USP;
    }
  }

  return IS_USP;
}


/*
 *=============================================================
 *
 *  Bidirectional Test
 *
 *=============================================================
 *
 * Idea: Perform a bidirectional search for a non-strong USP witness
 * in the puzzle U that starts from both the first row and the last
 * row of U.  It builds up partial functions for pi_2 and pi_3 and
 * stores the rows which have already been mapped to for each.  This
 * allows the subproblems to be specified by sets rather than by
 * permutations.  We use the fast implementation of sets as 64-bit
 * long below to encode the sets.
 *
 */


/*
 *-------------------------------------------------------------
 *  Pair of Sets Implementation
 *-------------------------------------------------------------
 *
 * Below is an implementation of pairs of sets over universe of size
 * at most 31.  U <= {0, 1, 2, ... ,30}.  Each pair p = (S2, S3), for
 * S2, S3 <= U, is represented as a 64-bit long.  For a given p, i in
 * U is in S2 iff the ith bit (ordered lowest to hight) of p is 1.
 * For a given p, i in U is in S3 iff the (i+32)th bit of p is 1.
 * (Note: Bit index 31 stores a flag used by check_usp_bi to determine
 * whether the sets have been constructed in a way that corresponds to
 * selecting identical permutations.  Bit index 63 is unused.)
 */

typedef long set_long;

/*
 * All operations are implemented as compiler macros so that they are
 * as fast as possible. (Remark: Considering refactoring as c++ inline
 * functions instead of macros for robustness.)  All operations are
 * O(1) and accomplished without using loops.
 */

/*
 * Computes bitmask of an element a from {0, 1, ..., 30} into S2 or
 * S3.
 */
#define TO_S2(a) (1L << (a))
#define TO_S3(a) (1L << ((a) + 32))

/*
 * Returns true iff both sets are empty.
 */
#define SETS_EMPTY(a) ((a) == 0L)

/*
 * Returns a pair of full sets; S2 = S3 = {0, 1, 2, ..., 30}.
 */
#define CREATE_FULL(n) ((long)(CREATE_FULL_INT(n)) | (((long)CREATE_FULL_INT(n)) << 32))
#define CREATE_FULL_INT(n) ((n) == 32 ? ~0 : ~((1 << 31) >> (31 - (n))))

/*
 * Returns a pair of empty sets; S2 = S3 = {}.
 */
#define CREATE_EMPTY() (0L)

/*
 * Returns the pair that is the complement of the given pair. p = (S2,
 * S3) -> -p = (-S2, -S3).
 */
#define COMPLEMENT(s,n) (CREATE_FULL(n) ^ s)

/*
 * Accessors that check whether an element from {0, 1, ..., 30} is
 * part of S2 or S3.
 */
#define MEMBER_S2(s,a) (((s) & TO_S2(a)) != 0L)
#define MEMBER_S3(s,a) (((s) & TO_S3(a)) != 0L)

/*
 * Mutators that insert an element into S2 or S3, with no effect if
 * the element is already present.
 */
#define INSERT_S2(s,a) ((s) | TO_S2(a))
#define INSERT_S3(s,a) ((s) | TO_S3(a))

/*
 * Mutators that delete an element into S2 or S3, with no effect if
 * the element is not present.
 */
#define DELETE_S2(s,a) ((s) & ~(TO_S2(a)))
#define DELETE_S3(s,a) ((s) & ~(TO_S3(a)))

/*
 * Functions that twiddle the bit at index 31 and checks whether it is
 * set.
 */
#define SET_ID(s) (INSERT_S3(s,31))
#define UNSET_ID(s) (DELETE_S3(s,31))
#define IS_ID(s) (MEMBER_S3(s,31))

/*
 * Function that displays the contents of a pair of sets to the console.
 */
void print_sets(set_long sets){

  printf("s2 = { ");
  for (int i = 0; i < 31; i++){

    if (MEMBER_S2(sets,i))
      printf("%d ",i);

  }
  printf("}, s3 = { ");
  for (int i = 0; i < 31; i++){

    if (MEMBER_S3(sets,i))
      printf("%d ",i);

  }
  printf("}\n");

}


// Function signatures -- so it will compile.
void find_witness_forward(int w, map<set_long,bool> * memo, int i1, set_long sets,
			  bool * tdm, bool is_id);
bool find_witness_reverse(int w, map<set_long,bool> * memo, int i1, set_long sets,
			  bool * tdm, bool is_id, map<set_long,bool> * opposite);




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
 * Checks whether 2d matchings occur from each of the three faces.
 * Returns true iff all of the 2d matchings are possible.
 */
bool has_2d_matchings(bool * tdm, int s){

  bool M[s * s];

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || tdm[i * s * s + j * s + r];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || tdm[j * s * s + r * s + i];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || tdm[r * s * s + i * s + j];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;

  return true;
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
check_t greedy_precheck(puzzle * p, int repeats){

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

  //printf("best_length = %d\n", best_length);

  return UNDET_USP;
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
check_t random_precheck(puzzle * p, int iter){

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


check_t check_usp_bi_inner(puzzle * p){

 // Create two maps that will store partial witnesses of U not being
  // a strong USP.
  //
  // The membership of a set pair p = (S2, S3) in forward_memo with t
  // = |S2| = |S3| means that there exists a 1-1 map from the first t
  // rows of U to S2 and S3 such that hits only false entries of
  // row_witness.  The membership of a set pair in reverse_memo is
  // similar, but for the _last_ t rows of U.
  //
  // Note that the value in forward_memo is not used, but the value in
  // reverse_memo indicates whether a witness has been found for that
  // subproblem.
  int s = p -> s;

  assert(s <= 31);

  map<set_long,bool> forward_memo;
  map<set_long,bool> reverse_memo;

  last_layer_forward = 0;
  size_forward = 0;
  checks_backward = 0;
  size_backward = 0;

  // Perform the first half of the search by filling in forward_memo
  // for the first s/2 rows of U.
  find_witness_forward(s, &forward_memo, 0, SET_ID(0L), p -> tdm, true);

  // Perform the second half of the search by filling in reverse_memo
  // for the second s/2 rows of U.  These partial mappings are
  // complemented and then looked up in forward_memo to check whether
  // they can be combined into a complete witness for U not being a
  // strong USP.

  bool res = !find_witness_reverse(s, &reverse_memo, s - 1, SET_ID(0L), p -> tdm, true, &forward_memo);
  
  return (res ? IS_USP : NOT_USP);

}

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a bidirectional algorithm.
 */
check_t check_usp_bi(puzzle * p){

  // Precompute s * s * s memoization table storing the mapping of
  // rows that witness that a partial mapping is consistent with U
  // being a strong USP.  For U to not be a strong USP there must a
  // way to select s false entries from the table whose indexes are
  // row, column, and slice disjoint.  This is the same first step as
  // in the unidirectional verison.  (MWA: I'm not sure why we
  // manually perform array indexing into a 3D array -- maybe it makes
  // passing the array more efficient?)
  compute_tdm(p);
  return check_usp_bi_inner(p);

}


/*
 * A memoized recursive function contructs partial witnesses of
 * non-strong USP for a s-by-k puzzle with given row_witness, from row
 * i1 to s/2 with sets storing the indexes already used by pi_2 and
 * pi_3.  is_id indicates whether the partial assignment to this point
 * is identity for all three permutations.
 */
void find_witness_forward(int s, map<set_long,bool> * memo, int i1,
			  set_long sets, bool * tdm, bool is_id){

  // Update the identity flag in sets, if the partial map is no longer
  // consistent with identity on all permutations.
  if (!is_id)
    sets = UNSET_ID(sets);

  // Base Case 1: Look to see whether we're already computed and
  // stored this answer in the memo table.  If so, return.
  map<set_long,bool>::const_iterator iter = memo -> find(sets);
  if (iter != memo -> end()){
    return;
  }

  // Base Case 2: We have processed enough rows, insert the final sets
  // of size floor(s/2) into the memo table and return.  Note that
  // value false stored in the memo table doesn't mean anything.
  if (i1 == (int)(floorf(s / 2.0))) {
    memo -> insert(pair<set_long,bool>(sets, false));
    last_layer_forward++;
    size_forward++;
    return;
  }

  // Recursive Case: There is more work to do.  Loop over all pairs
  // i2, i3 that are not already present in sets and check whether
  // tdm[i1][i2][i3] is true.  If so, recurse on the subproblem that
  // results from inserting i2 and i3 into sets, incrementing i1, and
  // updating the identity flag.
  for (int i2 = 0; i2 < s; i2++){
    if (MEMBER_S2(sets,i2))
      continue;
    for (int i3 = 0; i3 < s; i3++){
      if (MEMBER_S3(sets,i3))
	continue;
      if (tdm[i1 * s * s + i2 * s + i3] == false)
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets, i2), i3);

      find_witness_forward(s, memo, i1 + 1, sets2,
			   tdm, is_id && i1 == i2 && i2 == i3);
    }
  }

  // Insert the subproblem we just completed into the memo table, so
  // the computation will not be repeated.
  memo -> insert(pair<set_long,bool>(sets,false));
  size_forward++;
  return;
}


/*
 * A memoized recursive function contructs partial witnesses of
 * non-strong USP for a s-by-k puzzle with given row_witness, from row
 * i1 to s/2 with sets storing the indexes already used by pi_2 and
 * pi_3.  is_id indicates whether the partial assignment to this point
 * is identity for all three permutations.  Takes in a map containing
 * the memo table opposite from the forward search.  Return true iff
 * an witness that U is not a strong USP has been found.
 */
bool find_witness_reverse(int s, map<set_long,bool> * memo, int i1,
			  set_long sets, bool * tdm, bool is_id, map<set_long,bool> * opposite){

  // Update the identity flag in sets, if the partial map is no longer
  // consistent with identity on all permutations.
  if (!is_id)
    sets = UNSET_ID(sets);

  // Base Case 1: Look to see whether we're already computed and
  // stored this answer in the memo table.  If so, return that answer.
  map<set_long,bool>::const_iterator iter = memo -> find(sets);
  if (iter != memo -> end()){
    return iter->second;
  }

  // Base Case 2: We have processed enough rows, look for a complement
  // in the opposite table to form a complete witness.  If sets was
  // produced by identities, then the complement cannot be an identity
  // and still be a witness, because strong USP ignores the case that
  // all permutations are the same.
  if (i1 < (int)(floorf((s - 1) / 2.0))) {

    set_long sets_comp = COMPLEMENT(sets,s);
    bool found_pair = false;

    if (is_id) {
      // If sets is identity, it can only pair with non-identities.
      sets_comp = UNSET_ID(sets_comp);
    } else {
      // If sets is not identity, it pair with either identities or non-identites.
      iter = opposite -> find(sets_comp);
      if (iter != opposite -> end()){
	found_pair = true;
      }
      sets_comp = SET_ID(sets_comp);
    }

    iter = opposite -> find(sets_comp);
    if (iter != opposite -> end()){
      found_pair = true;
    }

    // Insert result in memo table.  If we didn't find a witness
    // looking at this subproblem, make a note of this in the memo
    // table so the computation is not repeated.  If we did find a
    // witness the computation will immediately return from each
    // recursive call in this case (and it wasn't really necessary to
    // insert it).
    memo -> insert(pair<set_long,bool>(sets, found_pair));
    if (!found_pair)
      checks_backward++;
    size_backward++;

    return found_pair;
  }

  // Recursive Case: There is more work to do.  Loop over all pairs
  // i2, i3 that are not already present in sets and check whether
  // tdm[i1][i2][i3] is true.  If so, recurse on the
  // subproblem that results from inserting i2 and i3 into sets,
  // incrementing i1, and updating the identity flag.
  for (int i2 = 0; i2 < s; i2++){
    if (MEMBER_S2(sets, i2))
      continue;
    for (int i3 = 0; i3 < s; i3++){
      if (MEMBER_S3(sets, i3))
	continue;
      if (tdm[i1 * s * s + i2 * s + i3] == false)
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets, i2), i3);

      if (find_witness_reverse(s, memo, i1 - 1, sets2, tdm,
			       is_id && i1 == i2 && i2 == i3, opposite)){
	// We've found a witness that U is not a strong USP.  Note
	// that we don't actually need to insert it in the memo table
	// as the computation will immediately return from each
	// recursive call in this case.
	memo -> insert(pair<set_long,bool>(sets, true));
	size_backward++;
	return true;

      }
    }
  }

  // We didn't find a witness looking at this subproblem, make a note
  // of this in the memo table so the computation is not repeated.
  memo -> insert(pair<set_long,bool>(sets,false));
  size_backward++;
  return false;

}



/*
 *=============================================================
 *
 *  Caching
 *
 *=============================================================
 *
 * Creates a cache of previously computed puzzles that stores whether
 * or not they are strong USPs.  Only caches relatively small puzzles
 * because of memory constraints.
 */

// Global variables storing cache data.
int const MAX_CACHE_S = 4;
int const MAX_CACHE_K = 20;
map<string,void *> * cache[MAX_CACHE_S+1][MAX_CACHE_K+1];
bool cache_valid[MAX_CACHE_S+1][MAX_CACHE_K+1];

/*
 * Converts a given puzzle U with s rows into a string.
 */
string U_to_string(puzzle_row U[], int s){

  ostringstream oss;
  oss << U[0];
  for (int i = 1; i < s; i++)
    oss << "|" << U[i];
  return oss.str();

}

/*
 * Returns whether s-by-k puzzles can be cached.
 */
bool can_cache(int s, int k){
  return (s >= 1 && s <= MAX_CACHE_S) && (k >= 1 && k <= MAX_CACHE_K);
}

/*
 * Returns whether the cache for s-by-k has been created.
 */
bool is_cached(int s, int k){

  if (!can_cache(s,k))
    return false;

  assert(!cache_valid[s][k]); // Triggers if cache is being accessed (which is it suppose to).

  return (cache_valid[s][k]);

}

/*
 * Returns whether the given s-by-k puzzle U is a strong USP.  The
 * puzzle must already have been computed and stored in the cache.
 */
bool cache_lookup(puzzle * p){

  assert(is_cached(p -> s, p -> k));

  // Trivally succeeds.
  if (p -> s == 1)
    return true;

  // Look up and return result.
  map<string,void*>::const_iterator iter = cache[p -> s][p -> k] -> find(U_to_string(p -> puzzle, p -> s));

  return (iter != cache[p -> s][p -> k] -> end());

}



/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Checks using SAT and MIP solvers in parallel threads.
 */
check_t check_SAT_MIP(puzzle * p){

  sem_t complete_sem;
  sem_init(&complete_sem, 0, 0);
  
  pthread_t th_MIP;
  pthread_t th_SAT;
  struct thread input_MIP;
  input_MIP.p = p;
  input_MIP.interrupt = false;
  input_MIP.complete_sem = &complete_sem;
  input_MIP.complete = false;
    
  struct thread input_SAT;
  input_SAT.p = p;
  input_SAT.interrupt = false;
  input_SAT.complete_sem = &complete_sem;
  input_SAT.complete = false;
  
  pthread_create(&th_SAT, NULL, SAT, (void *)&input_SAT);
  pthread_create(&th_MIP, NULL, MIP, (void *)&input_MIP);

  sem_wait(&complete_sem);

  long res = UNDET_USP;

  if(input_MIP.complete){
    input_SAT.interrupt = true;
    pthread_join(th_MIP, (void **)&res);
    pthread_join(th_SAT, NULL);
    return (check_t) res;
  } 

  if(input_SAT.complete){
    input_MIP.interrupt = true;
    pthread_join(th_SAT, (void **)&res);
    pthread_join(th_MIP, NULL);
    return (check_t) res;
  }

  assert(false == "SHOULDN'T GET HERE.");
  return (check_t) res;
}

/*
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Tries to pick the most efficient method.  Uses cache if present;
 * call init_cache() to use this feature).  Uses the bidirectional
 * search if s is large enough, and the unidirectional search
 * otherwise.
 */
check_t check(puzzle * p){

  int s = p -> s;
  int k = p -> k;
  
  if (is_cached(s,k))
    return (cache_lookup(p) ? IS_USP : NOT_USP);
  else if (s < 3)
    return check_usp_uni(p);
  else if (s < 8) {
    return check_usp_bi(p);
  } else {

    int iter = s * s * s;

    check_t res = random_precheck(p, iter);
    if (res != UNDET_USP)
      return res;

    if (s < 10){
      /*
      simplify_tdm(p);
      res = greedy_precheck(p -> tdm, p -> s, iter);
      if (res != 0)
	return res == 1;
      */
      return check_usp_bi_inner(p);
    } else {

      // XXX - This won't do anything for the SAT solver because it
      //doesn't take in row_witness.
      // XXX - May be incorrect.
      //simplify_tdm(p);

      res = greedy_precheck(p, iter);
      if (res != UNDET_USP)
	return res;

      res = check_SAT_MIP(p);
      assert(res != UNDET_USP);
      return res;
    }
  }
}



/*
 * A specialized function that determines whether the given 2-by-k
 * puzzle U is a strong USP.
 */
check_t check2(puzzle_row r1, puzzle_row r2, int k){

  int s = 2;
  puzzle p;
  puzzle_row U[s];
  bool tdm[s * s * s];
  p.puzzle = U;
  p.max_row = MAX_ROWS[k];
  p.tdm = tdm;
  p.s = s;
  p.k = k;
  
  U[0] = r1;
  U[1] = r2;

  return check(&p);

}

/*
 * A specialized function that determines whether the given 3-by-k
 * puzzle U is a strong USP.
 */
check_t check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k){

  int s = 3;
  puzzle p;
  puzzle_row U[s];
  bool tdm[s * s * s];
  p.puzzle = U;
  p.max_row = MAX_ROWS[k];
  p.tdm = tdm;
  p.s = s;
  p.k = k;
  
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  return check(&p);

}

/*
 * A specialized function that determines whether the given 4-by-k
 * puzzle U is a strong USP.
 */
check_t check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k){

  int s = 4;
  puzzle p;
  puzzle_row U[s];
  bool tdm[s * s * s];
  p.puzzle = U;
  p.max_row = MAX_ROWS[k];
  p.tdm = tdm;
  p.s = s;
  p.k = k;

  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  U[3] = r4;
  return check(&p);

}

/*
 * A specialized function that determines whether any pair of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
check_t check_row_pairs(puzzle * p){
  int s = p -> s;
  for (int r1 = 0; r1 < s-1; r1++){
    for (int r2 = r1+1; r2 < s; r2++){
      check_t res = check2(p -> puzzle[r1], p -> puzzle[r2], p -> k);
      if (res == UNDET_USP)
	return UNDET_USP;
      if (res == NOT_USP)
	return NOT_USP;
    }
  }
  return IS_USP;
}

/*
 * A specialized function that determines whether any triple of rows
 * prevent an s-by-k from being a strong USP.  Return true iff all
 * pairs of rows are valid.
 */
check_t check_row_triples(puzzle * p){

  int s = p -> s;
  for (int r1 = 0; r1 < s-2; r1++){
    for (int r2 = r1+1; r2 < s-1; r2++){
      for (int r3 = r2+1; r3 < s; r3++){
	check_t res = check3(p -> puzzle[r1],p -> puzzle[r2],p -> puzzle[r3],p -> k);
	if (res == UNDET_USP)
	  return UNDET_USP;
	if (res == NOT_USP)
	  return NOT_USP;
      }
    }
  }
  return IS_USP;
}

/*
 * Initialize the USP cache.  Precomputes whether each s-by-k puzzle
 * is a strong USP and stores the result in the cache.  Requires s <=
 * 4.  Idempotent.
 */

void init_cache(int s, int k){

  if (!can_cache(s, k)) {
    fprintf(stderr,"Error: Unable to make a cache of this size.\n");
    return;
  }

  if (is_cached(s,k)) {
    return;
  }

  printf("Creating %d-by-%d USP cache.\n", s, k);


  puzzle * p = create_puzzle(s, k);
  puzzle_row * U = p -> puzzle;
  puzzle_row max_row = p -> max_row;
  
  if (s >= 2){

    p -> s = 2;
    
    cache[2][k] = new map<string,void *>();

    for (U[0] = 0; U[0] < max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < max_row; U[1]++){
	if (check(p)) {
	  //cout << "Cached USP: " << U_to_string(U,2) << endl;
	  cache[2][k] -> insert(pair<string,void *>(U_to_string(U,2),NULL));
	}
      }
    }

  }

  if (s >= 3) {

    p -> s = 3;
    
    cache[3][k] = new map<string,void *>();

    for (U[0] = 0; U[0] < max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < max_row; U[2]++){
	  if (check(p)) {
	    cache[3][k] -> insert(pair<string,void *>(U_to_string(U,3),NULL));
	  }
	}
      }
    }
  }

  if (s >= 4){

    p -> s = 4;
    
    cache[4][k] = new map<string,void *>();

    for (U[0] = 0; U[0] < max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < max_row; U[2]++){
	  for (U[3] = U[2]+1; U[3] < max_row; U[3]++){
	    if (check(p)) {
	      cache[4][k] -> insert(pair<string,void *>(U_to_string(U,4),NULL));
	    }
	  }
	}
      }
    }

  }

  cache_valid[s][k] = true;
  printf("Created %d-by-%d USP cache.\n", s, k);

  destroy_puzzle(p);

}
