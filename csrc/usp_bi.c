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
#include "usp_bi.h"

using namespace std;


/*
 *=============================================================
 *
 *  Pair of Sets Implementation
 *
 *=============================================================
 *
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
 * A puzzle U is a set of length-k strings over {1,2,3}.  We say that
 * U is a strong uniquely solvable puzzle (USP) if for all pi_1, pi_2,
 * pi_3 in Symmetric(U), either pi_1 = pi_2 = pi_3 or there exists u
 * in U and i in [k] such that exactly two of (pi_1(u))_i = 1,
 * (pi_2(u))_i = 2, and (pi_3(u))_i = 3 hold.  
 *
 * While checking this condition One can assume without loss of
 * generality that pi_1 is the identity permutation, because only the
 * relative difference in the permutations is relevant.  In particular
 * if the inner condition holds (or doesn't) for pi_1, pi_2, pi_3, and
 * u, it also holds for 1, pi_1^-1 o pi_2, pi_1^-1 o pi_3, and
 * pi_1^-1(u).
 */

/* 
 * This is an alternative implementation of check_usp from usp.c.  It
 * uses c++ iterators with the built-in function next_permutation to
 * loop over all permutations.
 */
bool check_usp_uni(puzzle_row U[], int s, int k){

  // Build s * s * s memoization table storing the mapping of rows
  // that witness that a partial mapping is consistent with U being a
  // strong USP.  For U to not be a strong USP there must a way to
  // select s false entries from the table whose indexes are row,
  // column, and slice disjoint.
  bool check_memo[s][s][s];
  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	check_memo[i][j][r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }

  // The original statement of the strong USP property checks all 
  // Create two vectors for storing permutations over the set of s
  // rows.  The first permutation p_1 is implicitly the identity,
  // because U is a set.
  vector<int> p_2(s);
  vector<int> p_3(s);
  
  // Initialize p_2 to the identity permutation.
  for (int i = 0; i < s; i++) p_2[i] = i;

  // Loop over all permutations p_2.
  bool go1 = true;
  for (; go1 ; go1 = next_permutation(p_2.begin(),p_2.end())){

    // Initialize p_3 to the identity permutation.
    for (int i = 0; i < s; i++) p_3[i] = i;
    int go2 = true;

    // Loop over all permutations p_3.
    for (; go2 ; go2 = next_permutation(p_3.begin(),p_3.end())){
      if (ident(p_2,s) && ident(p_3,s)) continue;

      bool found = false;
      for (int i = 0; i < s; i++){
	if (check_memo[i][p_2[i]][p_3[i]]) {
	  found = true;
	  break;
	}
      }
      if (!found)
	return false;
    }     
  }

  return true;
}


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






// Generates front direction of search.
void find_witness_forward(int w, int k, map<set_long,bool> * memo, int i1, set_long sets, bool * RR, bool is_id){

  // Unset identity flag if last choices were distinct.
   if (!is_id)  
    sets = UNSET_ID(sets);

  map<set_long,bool>::const_iterator iter = memo -> find(sets);

  if (iter != memo -> end()){
    return;
  }

  if (i1 == (int)(floorf(w / 2.0))) {
    //print_sets(sets);
    memo -> insert(pair<set_long,bool>(sets,false)); // XXX - Value saved is meaningless.
    return;
  }

  for (int i2 = 0; i2 < w; i2++){
    if (MEMBER_S2(sets,i2)) 
      continue;
    for (int i3 = 0; i3 < w; i3++){
      if (MEMBER_S3(sets,i3)) 
	continue;
      if (RR[i1 * w * w + i2 * w + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets,i2),i3);     
      
      find_witness_forward(w,k,memo,i1+1,sets2,RR, is_id && i1 == i2 && i2 == i3);
    }
  }
  
  memo -> insert(pair<set_long,bool>(sets,false)); // XXX - Value saved is meaningless.
  return;
  
}


bool find_witness_reverse(int w, int k, map<set_long,bool> * memo, int i1, set_long sets, bool * RR, bool is_id, map<set_long,bool> * opposite){

  // Unset identity flag if last choices were distinct.
  if (!is_id) 
    sets = UNSET_ID(sets);
  
  map<set_long,bool>::const_iterator iter = memo -> find(sets);

  if (iter != memo -> end()){
    return iter->second;
  }

  if (i1 < (int)(floorf((w - 1)/2.0))) {
    //printf("w = %d i1 = %d\n", w,i1);
    //print_sets(sets);

    set_long sets_comp = COMPLEMENT(sets,w);
    //print_sets(sets_comp);
    bool found_pair = false;

    if (is_id) {
      sets_comp = UNSET_ID(sets_comp);
    } else {
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

    memo -> insert(pair<set_long,bool>(sets,found_pair));

    if (found_pair){
      //fprintf(stderr,"FOUND MATCH!\n");
    }
    return found_pair;
  }

  for (int i2 = 0; i2 < w; i2++){
    if (MEMBER_S2(sets,i2)) 
      continue;
    for (int i3 = 0; i3 < w; i3++){
      if (MEMBER_S3(sets,i3)) 
	continue;
      if (RR[i1 * w * w + i2 * w + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets,i2),i3);     

      if (find_witness_reverse(w,k,memo,i1-1,sets2,RR,is_id && i1 == i2 && i2 == i3, opposite)){
	memo -> insert(pair<set_long,bool>(sets,true));
	
	return true;
      }
      
    }
  }

  memo -> insert(pair<set_long,bool>(sets,false));
  return false;
  
}

bool check_usp_bi(puzzle_row U[], int w, int k){
  
  bool RR[w * w * w];
  
  for (int i = 0; i < w; i++){
    for (int j = 0; j < w; j++){
      for (int r = 0; r < w; r++){
	RR[i * w * w + j * w + r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }
  
  map<set_long,bool> forward_memo;
  map<set_long,bool> reverse_memo;
  
  find_witness_forward(w,k,&forward_memo,0,SET_ID(0L),RR,true);
  
  return !find_witness_reverse(w,k,&reverse_memo,w-1,SET_ID(0L),RR,true,&forward_memo);
  
}

map<string,void *> * cache[5];
int cache_size = -1;
puzzle_row local_max_row = -1;

string U_to_string(puzzle_row U[], int s){

  ostringstream oss;
  oss << U[0];
  for (int i = 1; i < s; i++)
    oss << "|" << U[i];
  return oss.str();

}

bool cache_lookup(puzzle_row U[], int s, int k){

  assert(s >= 1 && s <= 4);

  if (s == 1)
    return true;

  map<string,void*>::const_iterator iter = cache[s] -> find(U_to_string(U,s));

  return (iter != cache[s] -> end());

}

bool check(puzzle_row U[], int s, int k){
  
  if (cache_size >= s)
    return cache_lookup(U,s,k);
  else if (s >= 3)
    return check_usp_bi(U,s,k);
  else
    return check_usp_uni(U,s,k);

}


bool check2(puzzle_row r1, puzzle_row r2, int k){

  puzzle_row U[2];
  U[0] = r1;
  U[1] = r2;
  return check(U,2,k);

}

bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k){

  puzzle_row U[3];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  return check(U,3,k);
  
}

bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k){
  
  puzzle_row U[4];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  U[3] = r4;
  return check(U,4,k);

}

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
  
