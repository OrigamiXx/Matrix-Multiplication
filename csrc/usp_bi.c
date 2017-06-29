/* 
 * Module implementing several non-trivial methods for checking
 * whether a given puzzle is a strong USP.
 *
 * 1. Undirectional search for a witness that puzzle is not a strong USP.
 * 2. Bidirectional search for a witness that puzzle is not a strong USP.
 * 3. Hybrid search using 1 & 2 with the option to precompute and
 *    cache small puzzles.
 *
 * Uses it's own representation of sets and puzzles.
 *
 * Author: Matt.
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
#include "usp_bi.h"
#include "matching.h"

using namespace std;

/*
 *=============================================================
 *
 *  Helper Functions
 *
 *=============================================================
 */

/*
 * Prints an s-by-k puzzle U to the console.
 */
void printU(puzzle_row U[], int s, int k){
  for (int i =0; i < s; i++){
    int x = U[i];
    for (int j = 0; j < k; j++){
      printf("%d ",x % 3 + 1);
      x = (x - (x % 3)) / 3;
    }
    printf("\n");
  }
  printf("---\n");
}

/*
 * Returns true iff a length-k row that permutations map to u_1, u_2,
 * u_3, respectively, satisfies the inner condition of strong USPs.
 * It is false if this length-k row this mapping does not witness that
 * the puzzle is a strong USP.  Runtime is O(k).
 */
bool valid_combination(int u_1, int u_2, int u_3, int k){
  
  for (int i = 0; i < k; i++){
    int count = 0;
    int d1 = u_1 % 3;
    int d2 = u_2 % 3;
    int d3 = u_3 % 3;
    u_1 = (u_1 - d1) / 3;
    u_2 = (u_2 - d2) / 3;
    u_3 = (u_3 - d3) / 3;
    
    if (d1 == 0) count++;
    if (d2 == 1) count++;
    if (d3 == 2) count++;
    if (count == 2) return true;
  }
  return false;
}

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
bool check_usp_uni(puzzle_row U[], int s, int k){

  // Precompute s * s * s memoization table storing the mapping of
  // rows that witness that a partial mapping is consistent with U
  // being a strong USP.  For U to not be a strong USP there must a
  // way to select s false entries from the table whose indexes are
  // row, column, and slice disjoint.
  bool row_witness[s][s][s];
  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	row_witness[i][j][r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }

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

      // Check whether pi_1, pi_2, pi_3 touch only falses.
      bool found = false;
      for (int i = 0; i < s; i++){
	if (row_witness[i][pi_2[i]][pi_3[i]]) {
	  found = true;
	  break;
	}
      }

      if (!found)
	// pi_2 and pi_3 are a witness that U is not a strong USP.
	return false;
    }     
  }

  return true;
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
void find_witness_forward(int w, int k, map<set_long,bool> * memo, int i1, set_long sets,
			  bool * row_witness, bool is_id);
bool find_witness_reverse(int w, int k, map<set_long,bool> * memo, int i1, set_long sets,
			  bool * row_witness, bool is_id, map<set_long,bool> * opposite);




/*
 *  Reorders rows in an attempt to make fewer entries be generated.
 *  They can be arranged in increasing or decreasing order, or outside
 *  -> inside, or inside -> outside.
 */
void reorder_witnesses(bool * row_witness, int s, bool increasing, bool sorted){

  // Count the witnesses in each layer.
  int layer_counts[s];
  for (int i = 0; i < s; i++){
    layer_counts[i] = 0;
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	if (!row_witness[i * s * s + j * s + r])
	  layer_counts[i]++;
      }
    }
    printf("layer_count[%d] = %d\n", i, layer_counts[i]);
  }

  // Controls whether order is increasing or decreasing.
  int increase = (increasing ? 1 : -1);

  // Compute a permutate that orders layers as desired.
  int perm[s];
  bool row_witness2[s * s * s];
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
	row_witness2[ix] = row_witness[i * s * s + j * s + r];
      }
    }
  }

  /*
  // Check that layers are arranged.
  for (int i = 0; i < s; i++){
    layer_counts[i] = 0;
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	if (!row_witness2[i * s * s + j * s + r])
	  layer_counts[i]++;
      }
      printf("layer_count[%d] = %d\n", i, layer_counts[i]);
    }
  }
  */

  // Copy back to original matrix.
  memcpy(row_witness, row_witness2, s * s * s * sizeof(bool));

}


/*
 * Checks whether 2d matchings occur from each of the three faces.
 * Returns true iff all of the 2d matchings are possible. 
 */
bool has_2d_matchings(bool * row_witness, int s){

  bool M[s * s];

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || !row_witness[i * s * s + j * s + r];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || !row_witness[j * s * s + r * s + i];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;

  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      M[i * s + j] = false;
      for (int r = 0; r < s; r++){
	M[i * s + j] = M[i * s + j] || !row_witness[r * s * s + i * s + j];
      }
    }
  }

  if (!has_perfect_bipartite_matching(M, s))
    return false;  
    
  return true;
}

/*
 * Returns true iff it finds a witness that puzzle is not a strong USP.
 * False indicates the search was inconclusive.
 */
bool has_greedy_random_witness(bool * row_witnesses, int s, int repeats){

  bool failed = false;
  for (int n = 0; n < repeats && !failed; n++){
  
    set_long curr = CREATE_EMPTY();
    for (int i = 0 ; i < s && !failed; i++){
      
      int startj = lrand48() % s;
      int startk = lrand48() % s;
      
      int j = startj;
      int k = startk;
      bool lapped = false;
      failed = false;
      while (!failed){
	
	k++;
	while (MEMBER_S3(curr,k)) k++;
	if (k == s) {
	  k = 0;
	  while (MEMBER_S3(curr,k)) k++;
	  j++;
	  while (MEMBER_S2(curr,j)) j++; 
	  if (j == s){
	    if (lapped) {
	      // Past start twice, must not have found a valid entry.
	      failed = true;
	    }
	    lapped = true;
	    j = 0;
	    while (MEMBER_S2(curr,j)) j++; 
	  }

	}
	
	if (!row_witnesses[i * s * s + j * s + k]){
	  //assert(!MEMBER_S2(curr, j) && !MEMBER_S3(curr,k));
	  // Found a new edge to add.
	  curr = INSERT_S2(INSERT_S3(curr,k),j);
	  break;
	}
      }
      if (failed){
	printf("Failed to locate witness in greedy_random. Acheived: %d / %d\n", i, s);
	break;
      }
    }
    if (!failed)
      return true;
    failed = false;
  }
  return false;
}

// Some variables measuring performance.
int size_forward = 0;
int last_layer_forward = 0;
int size_backward = 0;
int checks_backward = 0;
int matching_decided = 0;
int greedy_decided = 0;

/* 
 * Determines whether the given s-by-k puzzle U is a strong USP.  Uses
 * a bidirectional algorithm.
 */
bool check_usp_bi(puzzle_row U[], int s, int k){

  // Precompute s * s * s memoization table storing the mapping of
  // rows that witness that a partial mapping is consistent with U
  // being a strong USP.  For U to not be a strong USP there must a
  // way to select s false entries from the table whose indexes are
  // row, column, and slice disjoint.  This is the same first step as
  // in the unidirectional verison.  (MWA: I'm not sure why we
  // manually perform array indexing into a 3D array -- maybe it makes
  // passing the array more efficient?)
  bool row_witness[s * s * s];
  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	row_witness[i * s * s + j * s + r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }
  
  // Rearrange row_witness in the hope it makes the search faster.
  //reorder_witnesses(row_witness, s, true, true);
  
  /*
  // Checks that all 2d matchings exist on projected faces, a
  // necessary condition to not be a strong USP.
  bool precheck_res = !has_2d_matchings(row_witness, s);
  if (precheck_res) {
    matching_decided++;
    printf("matching_decided = %d\n", matching_decided);
    return true;
  }
  */

  if (has_greedy_random_witness(row_witness, s,10)){
    greedy_decided++;
    printf("greedy_decided = %d\n", greedy_decided);
    return false;
  }
  
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
  map<set_long,bool> forward_memo;
  map<set_long,bool> reverse_memo;

  last_layer_forward = 0;
  size_forward = 0;
  checks_backward = 0;
  size_backward = 0;
  
  // Perform the first half of the search by filling in forward_memo
  // for the first s/2 rows of U.
  find_witness_forward(s, k, &forward_memo, 0, SET_ID(0L), row_witness, true);

  // Perform the second half of the search by filling in reverse_memo
  // for the second s/2 rows of U.  These partial mappings are
  // complemented and then looked up in forward_memo to check whether
  // they can be combined into a complete witness for U not being a
  // strong USP.
  
  bool res = !find_witness_reverse(s, k, &reverse_memo, s - 1, SET_ID(0L), row_witness, true, &forward_memo);

  //if (precheck_res == 1 && !res)
  //  printf("Error: precheck and full check disagree!\n"); 

  printf("Forward memo table ratio: %d / %d\n", last_layer_forward, size_forward);
  printf("Backward failed checks: %d / %d\n",checks_backward, size_backward);
  
  return res;
  
  
}

/*
 * A memoized recursive function contructs partial witnesses of
 * non-strong USP for a s-by-k puzzle with given row_witness, from row
 * i1 to s/2 with sets storing the indexes already used by pi_2 and
 * pi_3.  is_id indicates whether the partial assignment to this point
 * is identity for all three permutations.
 */
void find_witness_forward(int s, int k, map<set_long,bool> * memo, int i1,
			  set_long sets, bool * row_witness, bool is_id){

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
  // row_witnesses[i1][i2][i3] is false.  If so, recurse on the
  // subproblem that results from inserting i2 and i3 into sets,
  // incrementing i1, and updating the identity flag.
  for (int i2 = 0; i2 < s; i2++){
    if (MEMBER_S2(sets,i2)) 
      continue;
    for (int i3 = 0; i3 < s; i3++){
      if (MEMBER_S3(sets,i3)) 
	continue;
      if (row_witness[i1 * s * s + i2 * s + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets, i2), i3);     
      
      find_witness_forward(s, k, memo, i1 + 1, sets2,
			   row_witness, is_id && i1 == i2 && i2 == i3);
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
bool find_witness_reverse(int s, int k, map<set_long,bool> * memo, int i1,
			  set_long sets, bool * row_witness, bool is_id, map<set_long,bool> * opposite){
  
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
  // row_witnesses[i1][i2][i3] is false.  If so, recurse on the
  // subproblem that results from inserting i2 and i3 into sets,
  // incrementing i1, and updating the identity flag.
  for (int i2 = 0; i2 < s; i2++){
    if (MEMBER_S2(sets, i2)) 
      continue;
    for (int i3 = 0; i3 < s; i3++){
      if (MEMBER_S3(sets, i3)) 
	continue;
      if (row_witness[i1 * s * s + i2 * s + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets, i2), i3);     

      if (find_witness_reverse(s, k, memo, i1 - 1, sets2, row_witness,
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
map<string,void *> * cache[5];
int cache_size = -1;
puzzle_row local_max_row = -1;

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
 * Returns whether the given s-by-k puzzle U is a strong USP.  The
 * puzzle must already have been computed and stored in the cache.
 */
bool cache_lookup(puzzle_row U[], int s, int k){

  assert(s >= 1 && s <= 4);

  // Trivally succeeds.
  if (s == 1)
    return true;

  // Look up and return result.
  map<string,void*>::const_iterator iter = cache[s] -> find(U_to_string(U,s));

  return (iter != cache[s] -> end());

}

/* 
 * Determines whether the given s-by-k puzzle U is a strong USP.
 * Tries to pick the most efficient method.  Uses cache if present;
 * call init_cache() to use this feature).  Uses the bidirectional
 * search if s is large enough, and the unidirectional search
 * otherwise.
 */
bool check(puzzle_row U[], int s, int k){
  
  if (cache_size >= s)
    return cache_lookup(U,s,k);
  else if (s >= 3)
    return check_usp_bi(U,s,k);
  else
    return check_usp_uni(U,s,k);

}

/* 
 * A specialized function that determines whether the given 2-by-k
 * puzzle U is a strong USP.
 */
bool check2(puzzle_row r1, puzzle_row r2, int k){

  puzzle_row U[2];
  U[0] = r1;
  U[1] = r2;
  return check(U,2,k);

}

/* 
 * A specialized function that determines whether the given 3-by-k
 * puzzle U is a strong USP.
 */
bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k){

  puzzle_row U[3];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  return check(U,3,k);
  
}

/* 
 * A specialized function that determines whether the given 4-by-k
 * puzzle U is a strong USP.
 */
bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k){
  
  puzzle_row U[4];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  U[3] = r4;
  return check(U,4,k);

}

/*
 * Initialize the USP cache.  Precomputes whether each s-by-k puzzle
 * is a strong USP and stores the result in the cache.  Requires s <=
 * 4.
 */

void init_cache(int k, int s){
  
  assert(s <= 4);

  //printf("Creating USP cache.\n");

  local_max_row = (puzzle_row)pow(3,k);

  int curr_size = cache_size;
  cache_size = -1;
  for (int x = 2; x <= curr_size; x++){
    delete cache[x];
  }

  puzzle_row U[4];

  if (s >= 2){
    
    cache[2] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	if (check(U,2,k)) {
	  //cout << "Cached USP: " << U_to_string(U,2) << endl;
	  cache[2] -> insert(pair<string,void *>(U_to_string(U,2),NULL));
	}
      }
    }
    
  } 

  if (s >= 3) {
    
    cache[3] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < local_max_row; U[2]++){
	  if (check(U,3,k)) {
	    cache[3] -> insert(pair<string,void *>(U_to_string(U,3),NULL));
	  }
	}
      }	
    }
  }
  
  if (s >= 4){
    
    cache[4] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < local_max_row; U[2]++){
	  for (U[3] = U[2]+1; U[3] < local_max_row; U[3]++){
	    if (check(U,4,k)) {
	      cache[4] -> insert(pair<string,void *>(U_to_string(U,4),NULL));
	    }
	  }
	}
      }
    }	
    
  }
  
  cache_size = s;

  //printf("Created USP cache up to size = %d\n",cache_size);
  
}
  
