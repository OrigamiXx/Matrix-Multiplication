/*
 * Implementation of SUSP checking and search based on PiDD.
 *
 * Summer 2019.
 */

#include "PiDD.hpp"
 
using namespace std; 

#include <cstdio> 
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <functional>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include "puzzle.h"
#include "checker.h"

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define ACCESS(e, c) (3*(e) + (c))

/*
 * Returns PiDD of all perms over indexes from start to end (inclusive).
 */
PiDD all_perms(int start, int end){
  
  PiDD id = PiDD_Factory::make_identity();  

  PiDD res = id;
  
  for (int a = start + 1; a <= end; a++){    
    for (int b = a - 1; b >= start; b--){
      PiDD curr = id | (id * (transpose){(uint8_t)a, (uint8_t)b});  
      res = res * curr;   
    }  
  }
  
  return res;

}


/*
 * Returns PiDD of all pairs of perms over indexes from start to end
 * couple with n indicating the length subdomains.  Applies from the
 * left to the given PiDD init incrementally for efficiency.  Results
 * in large PiDDs because the coupling and ordering of variables are
 * not complementary.
 */

PiDD all_perms_coupled(int start, int end, int n, PiDD init){
  
  PiDD id = PiDD_Factory::make_identity();  
  
  for (int a = start + 1; a <= end; a++){    
    for (int b = a - 1; b >= start; b--){
      PiDD curr = id | (id * (transpose){(uint8_t)a, (uint8_t)b} * (transpose){(uint8_t)(a + n), (uint8_t)(b+n)});
      init = curr * init;
    }  
  }
  
  return init;

}


/* 
 * Returns PiDD of all pairs of perms resulting from automorphisms of
 * c1 1 rows, c2 2 rows, and c3 3 rows of a column puzzle.  If
 * coupled, the automorphisms are pairs on the pi2 and pi3.  Applied
 * from the left to the PiDD init. 
*/
PiDD all_automorphisms(int c1, int c2, int c3, bool coupled, PiDD init){
 
  int s1 = 0;
  int e1 = s1 + c1 - 1;
  int s2 = e1 + 1;
  int e2 = s2 + c2 - 1;
  int s3 = e2 + 1;
  int e3 = s3 + c3 - 1;

  int n = c1 + c2 + c3;

  if (!coupled){

    PiDD dd1 = all_perms(s1, e1);
    PiDD dd2 = all_perms(s2, e2);
    PiDD dd3 = all_perms(s3, e3);
    
    PiDD dd4 = all_perms(s1+n, e1+n);
    PiDD dd5 = all_perms(s2+n, e2+n);
    PiDD dd6 = all_perms(s3+n, e3+n);
  
    return (dd1 * dd2 * dd3) * (dd4 * dd5 * dd6) * init;
    
  } else {
    
    init = all_perms_coupled(s1, e1, n, init);
    init = all_perms_coupled(s2, e2, n, init);
    init = all_perms_coupled(s3, e3, n, init);

    return init;
  }
  
}

// Array of rows overlaps to avoid when applying pi2 and pi3 to a
// column puzzle.  Rows with exactly two matching 1 in 1, 2 in 2, 3 in
// 3 columns are excluded.  Used by bad_perm to generate kernel of the
// bad permutations pairs.
int abcs[21] =
  {0,   // 111
   1,   // 112 
   //2,   // 113 X
   //3,   // 121 X
   //4,   // 122 X
   5,   // 123
   6,   // 131  
   7,   // 132
   //8,   // 133 X
   9,   // 211  // Index 5
   10,  // 212
   11,  // 213
   12,  // 221
   13,  // 222 
   //14,  // 223 X
   15,  // 231
   16,  // 232
   17,  // 233
   18,  // 311 // Index 13
   19,  // 312
   20,  // 313
   21,  // 321
   22,  // 322
   //23,  // 323 X
   24,  // 331
   25,  // 332
   26   // 333
  };


/* 
 * A memoized recursive function that helps compute the rigid
 * permutations of a column is cs values of each type of entry
 * remaining.  s is the total number of rows, s_left is the number of
 * rows that haven't been specified.  abc_ix indicate the next type of
 * row to attempt to add according to the array above.
 */

PiDD bad_perm_helper(int abc_ix, int cs[9], int s, int s_left,
		     unordered_map<string, PiDD> &memo){

  // Generate memo key and check if result is already known.
  ostringstream ss;
  ss << abc_ix;
  for (int i = 0; i < 9; i++)
    ss << "|" << cs[i];
  string key = ss.str();
  auto it = memo.find(key);
  if (it != memo.end()) 
    return (it -> second);

  // Initially no permutation pairs.   
  PiDD res = PiDD_Factory::make_empty();

  if (s_left == 0){
    // Everything is already set, so we return just identity.
    res = PiDD_Factory::make_identity();
  } else if (abc_ix >= 21 && s_left != 0) {
    // Impossible to complete puzzle, return empty.
    res = PiDD_Factory::make_empty();
  } else if ((abc_ix >= 5 && cs[ACCESS(0,0)] != 0)
	     || (abc_ix >= 13 && (cs[ACCESS(0,0)] != 0 || cs[ACCESS(1,0)] != 0))) {
    // Impossible to complete puzzle, return empty.
    res = PiDD_Factory::make_empty();
  } else {
    // Continue to fill out pi2 and pi3.

    // Determine target entries.
    int abc = abcs[abc_ix];
    int c = abc % 3;
    int b = (int)((abc - c) / 3) % 3;
    int a = (int)(abc / 9);
    
    // Determine maximum number of rows abcs that can be added.
    int max_count = MIN(MIN(cs[ACCESS(a,0)], cs[ACCESS(b,1)]), cs[ACCESS(c, 2)]);

    // Try all possible number of such rows.
    for (int m = 0; m <= max_count; m++){

      // Update the counts for entries available in each column.
      int cs2[9];
      memcpy(cs2, cs, sizeof(int) * 9);
      cs2[ACCESS(a,0)] -= m;
      cs2[ACCESS(b,1)] -= m;
      cs2[ACCESS(c,2)] -= m;

      // Recursively compute the bad_permutations for the remaining rows.
      PiDD dd = bad_perm_helper(abc_ix + 1, cs2, s, s_left - m, memo);

      // Combine with accumulated permutations if something was found.
      if (dd != PiDD_Factory::make_empty()){

	// Construct permutations make m rows abc.
	int L[3] = {a, b, c};
	int perm_list[2*s];

	for (int i = 0; i < 2 * s; i++){
	  perm_list[i] = i;
	}
	  
	for (int j = 1; j <= 2; j++){
	  int start = s - s_left;
	  int shift = (j - 1) * s;
	  int end = start;
	  
	  if (L[j] + 1 == 2) {
	    end = start + cs[ACCESS(0,j)];
	  } else if (L[j] + 1 == 3){
	    end = start + cs[ACCESS(0,j)] + cs[ACCESS(1,j)];
	  }
	  
	  for (int i = start; i < end; i++)
	    perm_list[i + shift] = i + m + shift;
	  
	  for (int i = end; i < end + m; i++)
	    perm_list[i + shift] = start + (i - end) + shift;
	  
	}

	// Apply this permutation to the result of the recursive call.
	perm * pi = create_perm_array(perm_list, 2*s);
	PiDD dd2 = PiDD_Factory::make_singleton(pi);
	dd2 = dd * dd2;
	destroy_perm(pi);

	// Union the recursive permutations with the accumulated results.
	res = res | dd2;
      }	
    }
  } 

  // Store result in memo table for future use.
  memo.insert(make_pair(key, res));
  
  return res;
}


/* 
 * Returns the PiDD representing the set of pairs of permutations pi2,
 * pi3 that make the column p not a SUSP.  
 */
PiDD bad_perms(puzzle * p){ 

  // Ensure columns only.
  assert(p -> k == 1); 

  int s = p -> s;

  // Compute number of occurrences of each entry in the column.
  int c[3] = {0,0,0};
  for (int r = 0; r < s; r++)
    c[get_entry(p, r, 0) - 1] += 1;

  // Compute permutations to convert p to sorted and back.
  int sort_perm_list[2*s];
  int sort_perm_inv_list[2*s];
  int c_sort[3] = {0, c[0], c[0] + c[1]};

  for (int r = 0; r < s; r++){
    int entry = get_entry(p, r, 0) - 1;
    sort_perm_list[r] = c_sort[entry];
    sort_perm_list[r + s] = c_sort[entry] + s;
    sort_perm_inv_list[c_sort[entry]] = r;
    sort_perm_inv_list[c_sort[entry] + s] = r + s;
    c_sort[entry] += 1;
  }

  perm * sort_perm = create_perm_array(sort_perm_list, 2*s);
  PiDD sort_dd = PiDD_Factory::make_singleton(sort_perm);
  destroy_perm(sort_perm);

  perm * sort_perm_inv = create_perm_array(sort_perm_inv_list, 2*s);
  PiDD sort_inv_dd = PiDD_Factory::make_singleton(sort_perm_inv);
  destroy_perm(sort_perm_inv);

  // Construct set of automorphisms of the sorted p.
  //printf("Construct automorphisms\n");
  PiDD input_auto = all_automorphisms(c[0], c[1], c[2], false, PiDD_Factory::make_identity());
  //printf("Input done\n");
  //printf("Output deferred\n");
  //input_auto.print_perms();
  //output_auto.print_perms();

  // Set up parameters and memo table to start recursion.
  int cs[9] = {c[0], c[0], c[0], c[1], c[1], c[1], c[2], c[2], c[2]};
  int abc_ix = 0;
  bool valid = true;
  unordered_map<string, PiDD> memo;

  // Recursively compute the bad permutations on the sorted p,
  // ignoring the automorphisms.
  //printf("Compute all bad perms\n");
  PiDD res = bad_perm_helper(abc_ix, cs, s, s, memo);

  // Construct the full set by applying independent row automorphisms
  // to both pi2 and pi3, conjugating the result by coupled
  // automorphisms of the output rows, and finally conjugating by the
  // sorting.  Association of the operations is the one that appeared
  // to have the least permformance overhead.  The coupled conjugation
  // by the output rows is by far the slowest part, seemingly because
  // the set of couple row automorphisms does not have a small
  // representation by a PiDD because of the choice to store pairs of
  // permutations by the disjoint union of two copies of the universe
  // (of rows).  The inherent variable ordering of PiDDs seems
  // incompatible with this.
  res = res * (input_auto * sort_dd);
  //printf("Combined rhs\n");
  res = all_automorphisms(c[0], c[1], c[2], true, res);
  //printf("Combined all_auto\n");
  res = sort_inv_dd * res;
  //printf("Combined lhs\n");
  
  return res;
  
}


/*
 * Returns IS_USP if p is a strong USP, and NOT_USP otherwise.  Uses
 * PiDDs to represent the sets of bad permutations for each column and
 * then verifies that the intersection of such sets is the singleton
 * set with the identity.  Not particularly efficient for s >= 10.
 */
 check_t check_PiDD(puzzle * p){

  PiDD P = all_automorphisms(p -> s, 0, 0, false, PiDD_Factory::make_identity()); 
  
  for (unsigned int c = 0; c < p -> k; c++){
    puzzle * p2 = create_puzzle(p -> s, 1);

    for (unsigned int r = 0; r < p -> s; r++){
      set_entry(p2, r, 0, get_entry(p, r, c));
    }
    PiDD P2 = bad_perms(p2);
    P = P & P2;
    //P.print_stats();
    destroy_puzzle(p2);
  }

  if (P == PiDD_Factory::make_identity())
    return IS_USP;
  else
    return NOT_USP;
}


// ======================================================================
//
// Search
//
// ======================================================================

PiDD minimize_intersection(PiDD S1, PiDD S2, int s){
  // Greedy intersection minimization by apply incremental transposes
  // to S2.  Returns resulting minimum PiDD.  Oddly, it's very
  // ineffective, and so has been disabled.

  PiDD S = S1 & S2;
  return S; // XXX - disabled.
  PiDD min_S = S;
  PiDD min_S2 = S2;

  S.print_stats();
  
  bool progress = true;
  while (progress){
    progress = false;
    
    for (uint8_t a = 1; a < s; a++){
      for (uint8_t b = 0; b < a; b++){
	PiDD T1 = (PiDD_Factory::make_identity() * (transpose){a, b} * (transpose){a+s, b+s});
	PiDD S3 = T1 * S2 * T1;
	PiDD S4 = S1 & S3;
	if (min_S.size() > S4.size()) {
	  min_S2 = S3;
	  min_S = S4;
	}
      }
    }

    if (S != min_S) {
      printf("Progress!\n");
      progress = true;
      S2 = min_S2;
      S = min_S;
    }
    S.print_stats();
  }

  return min_S;

}

/* 
 * Attempts to generate a large SUSP, by intersecting the bad perms
 * for puzzle columns and greedily extending puzzles by columns that
 * result in the fewest bad perms.  
 *
 * Does not work as well as hoped.  Does not generate puzzles with
 * fewest number of columns per row.  Attempts to reduce the number of
 * next columns considered by modding out by automorphisms and then
 * search for the automorphism which reduces the size of the
 * intersection.
 *
 * XXX - A better algorithm is required to make this approach
 * effective.  Though it doesn't seem worthwhile to attempt, because
 * the overall greedy strategy doesn't work that well.  Some other
 * property of the sets of bad perms seems necessary to improve the
 * situation.  Large node size for PiDDs might be a better indicator. 
 */
void search_PiDD(int s){

  PiDD cols[(s+1)][(s+1)];
  puzzle * col = create_puzzle(s, 1); 

  // Construct the bad perms for all columns of length s, up to row
  // reordering.
  float ave_size = 0;
  float ave_nodes = 0;
  int count = 0;
  for (int c1 = 0; c1 <= s; c1++){
    for (int c2 = 0; c2 <= s - c1; c2++){
      // c1 + c2 + c3 = s
      for (int r = 0; r < s; r++){
	int e = 0;
	if (r < c1) 
	  e = 1;
	else if (r < c1 + c2)
	  e = 2;
	else
	  e = 3; 
	set_entry(col, r, 0, e);
      }
      
      cols[c1][c2] = bad_perms(col);
      print_puzzle(col);
      cols[c1][c2].print_stats();
      ave_size += cols[c1][c2].size();
      ave_nodes += cols[c1][c2].node_size();
      count++;
    }
  }

  // Create priority queue of puzzles of two columns to consider.
  // Puzzles with fewer bad perms get higher priority.
  auto cmp = [](PiDD left, PiDD right) { return left.size() > right.size(); }; // STL uses highest priority.
  priority_queue<PiDD, std::vector<PiDD>, decltype(cmp)> * queue
    = new priority_queue<PiDD, std::vector<PiDD>, decltype(cmp)>(cmp); 
  
  printf("ave_size = %.1f, ave_nodes = %.1f\n", ave_size / count, ave_nodes / count);  
  PiDD min_S = cols[s][0];
  for (int c1 = 0; c1 <= s; c1++){
    for (int c2 = 0; c2 <= s - c1; c2++){
      for (int d1 = 0; d1 <= s; d1++){
	for (int d2 = 0; d2 <= s - d1; d2++){
	  PiDD S = minimize_intersection(cols[c1][c2], cols[d1][d2], s);
	  queue -> push(S);
	}
      }
    }
  }

  int PUZZLES_TO_TRY = 100;
  
  // Continue to add highest priority columns to form new puzzles.
  priority_queue<PiDD, std::vector<PiDD>, decltype(cmp)> * next_queue
    = new priority_queue<PiDD, std::vector<PiDD>, decltype(cmp)>(cmp); 
  
  int k = 2;
  while(queue -> top().size() != 1){
    printf("--------------------------------------------------------------\n");
    printf("min_size = %lu\n", queue->top().size());
    
    for (int i = 0; i < PUZZLES_TO_TRY && !queue -> empty(); i++){
      
      PiDD D = queue -> top();
      queue -> pop();
      for (int c1 = 0; c1 <= s; c1++){
	for (int c2 = 0; c2 <= s - c1; c2++){
	  
	  PiDD S = minimize_intersection(D, cols[c1][c2], s);
	  next_queue -> push(S);
	}
      }
    
    }
    
    delete queue;
    queue = next_queue;
    next_queue = new priority_queue<PiDD, std::vector<PiDD>, decltype(cmp)>(cmp); 
    
    k++;
  }

  printf("Found (%d, %d)-SUSP!\n", s, k);
  
  //return NULL;  // XXX - doesn't currently maintain or return the
    		  // found puzzle.  Though it could be added without
		  // much trouble.
}
