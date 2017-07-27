#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "usp_bi.h"
#include <time.h>
#include "3DM_to_SAT.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;


/* This file explores an experimental recursive construction of strong
   USPs we call "twisting".

   Given a s1-by-k1 strong USP p1, and s2-by-k2 strong USP p2, with k1
   >= k2.  A twist of these is any (s1+s2)-by-(k1+1) puzzle of the
   form diagrammed below.  

  +-----+a
  |     |a
  | p1  |a
  |     |a
  +-----+a
  +----+*b
  | p2 |*b
  +----+*b

  The last column has a, b in {1,2,3} with a != b.  The entries
  labeled with * may take any value.  The entries in columns of p2 + *
  are permuted arbitrarily relative to the columns of p1.  The
  elements {1,2,3} may be permuted in p2.

  The are two versions of the twisting algorithm, one which tests all
  possible twists until a valid one is found, and one that uniformly
  selects random twists until a valid one is found or a number of
  attempts have been made.

  What twists attempted are determine by the parameters below.  
*/
// The column permutations considered. 
bool const PERM_MODE =
  //false; // Tries only identity permutation of p2 + * -- not very effective.
  true;   // Tries all permutations of p2 + *.

bool const EL_PERM_MODE =
  //false; // Tries only identity permutation of the elements {1,2,3} in p2.
  true; // Tries all permutations of the elements {1,2,3} in p2.

// Restrictions on what go in *.  
int const ARB_MODE =
  //0; // A constant 1, 2, or 3.
  //1; // A duplicate of a column in p2. (If * is more than one column the same duplicate will be used.)
  //2;   // A constant 1, 2, or 3, or a duplicate of a column in p2.  
  3; // No restriction. 

// How many columns that strong USPs constructed by twisting must have with only two elements of {1,2,3}.
// Only <= 1 is guaranteed by construction, but 2 and 3 may work.  4 won't work.
int const N_SPECIAL_COLS = 2;

// Number of iterations to perform randomized twisting before giving up.
int const POP_ITER = 1000;

// Sizes vs. k of strong USPs that exists (except for 21-by-7 that is just conjectured);
int ss[8] = {0,   1,   2,     3,      5,      8,    13,    21};
// Number of strong USPs to generate for each k
int ns[8] = {0, 100, 500, 10000,     500,   100,   10,     1};


bool puzzle_has_at_least_n_two_columns(puzzle * p, int n){

  int found = 0;
  for (int c = 0; c < p -> column; c++){
    int counts[3] = {0,0,0};
    for (int r = 0; r < p -> row; r++){
      counts[get_column_from_row(p -> puzzle[r], c) - 1]++;
    }

    int missing = (counts[0] == 0) + (counts[1] == 0) + (counts[2] == 0);
    if (missing == 1)
      found++;
  }

  return found >= n;
  
}

void compute_twist_params(puzzle * p, puzzle *p1, puzzle *p2, int *num_last,
			  int *num_arb, int *num_perm, int *num_el_perm){

  int s = p -> row;
  int k = p -> column;
  int * puz = p -> puzzle;
  int k1 = p1 -> column;
  int s1 = p1 -> row;
  int k2 = p2 -> column;
  int s2 = p2 -> row;
  int dk = k1 - p2 -> column;

  *num_last = 6;

  *num_arb =
    (ARB_MODE == 0 ? 3 :
     (ARB_MODE == 1 ? k2 :
      (ARB_MODE == 2 ? k2 + 3 :
       (long)pow(3, s2 * dk))));


  if (PERM_MODE) {
    *num_perm = 1;
    for (int i = 1; i <= k1; i++) *num_perm *= i;
  } else {
    *num_perm = 1;
  }

  if (EL_PERM_MODE)
    *num_el_perm = 6;
  else
    *num_el_perm = 1;
  
}

bool apply_twist(puzzle * p, puzzle * p1, puzzle * p2, int last, int arb, int perm_id, int el_perm_id) {
  
  int s = p -> row;
  int k = p -> column;
  int * puz = p -> puzzle;
  int k1 = p1 -> column;
  int s1 = p1 -> row;
  int k2 = p2 -> column;
  int s2 = p2 -> row;
  int dk = k1 - p2 -> column;

  int x, y;
  if (last < 2) {
    x = 1;
    y = (last == 0 ? 2 : 3);
  } else if (last < 4){
    x = 2;
    y = (last == 2 ? 1 : 3);
  } else {
    x = 3;
    y = (last == 4 ? 1 : 2);
  }
  
  perm * pi = create_perm_identity(k1);
  
  for (int j = 0; j < perm_id; j++)
    next_perm(pi);

  perm * rho = create_perm_identity(3);
  
  for (int j = 0; j < el_perm_id; j++)
    next_perm(rho);
  
  // Copy p1 into p.
  for (int c = 0; c < k1; c++){
    for (int r = 0; r < s1; r++){
      puz[r] = set_entry_in_row(puz[r], c, get_column_from_row(p1 -> puzzle[r], c));
    }
  }
  
  // Copy p2 into p according to pi.
  for (int c = 0; c < k2; c++){
    for (int r = 0; r < s2; r++){
      puz[r + s1] =
	set_entry_in_row(puz[r + s1],
			 apply_perm(pi, c),
			 apply_perm(rho, get_column_from_row(p2 -> puzzle[r], c) - 1) + 1);
    }
  }
  
  int id = 0;
  for (int c = k2; c < k1; c++){
    for (int r = 0; r < s2; r++){
      puz[r + s1] =
	set_entry_in_row(puz[r + s1],
			 apply_perm(pi,c),
			 (ARB_MODE == 0 ? arb + 1 :
			  (ARB_MODE == 1 ? apply_perm(rho, get_column_from_row(p2 -> puzzle[r], arb)-1)+1 :
			   (ARB_MODE == 2 ? (arb < 3 ? arb + 1 :
					     apply_perm(rho, get_column_from_row(p2 -> puzzle[r], arb - k2)-1)+1) :
			    get_column_from_row(arb, id)))));

      id++;
    }
  }
  
  for (int r = 0; r < s1; r++){
    puz[r] = set_entry_in_row(puz[r], k1, x);
  }
  
  for (int r = 0; r < s2; r++){
    puz[r + s1] = set_entry_in_row(puz[r + s1], k1, y);
  }
  
  destroy_perm(pi);
  destroy_perm(rho);

  if (!puzzle_has_at_least_n_two_columns(p, N_SPECIAL_COLS))
    return false;


  if (check_row_triples(puz, s, k)){
    if (check(puz, s, k)) {
      return true;
    }
  }

  return false;

}

bool random_twist(puzzle * p, puzzle * p1, puzzle * p2, int iter){

  int num_last, num_arb, num_perm, num_el_perm;
  
  compute_twist_params(p, p1, p2, &num_last, &num_arb, &num_perm, &num_el_perm);

  for (int i = 0; i < iter; i++){

    int last = lrand48() % num_last;
    int arb = lrand48() % num_arb;
    int perm_id = lrand48() % num_perm;
    int el_perm_id = lrand48() % num_el_perm;

    if (apply_twist(p, p1, p2, last, arb, perm_id, el_perm_id)){
      return true;
    }
    
  }
  
  return false;

}

bool full_twist(puzzle * p, puzzle * p1, puzzle * p2){

  int num_last, num_arb, num_perm, num_el_perm;
 
  compute_twist_params(p, p1, p2, &num_last, &num_arb, &num_perm, &num_el_perm);

  for (int last = 0; last < num_last; last++) {
    for (int arb = 0; arb < num_arb; arb++) {
      for (int perm_id = 0; perm_id < num_perm; perm_id++) {
	for (int el_perm_id = 0; el_perm_id < num_el_perm; el_perm_id++) {
	  if (apply_twist(p, p1, p2, last, arb, perm_id, el_perm_id)){
	    return true;
	  }
	}
      }
    }
  }
  return false;

}


int const MAX_K = 10;

string puzzle_to_string(puzzle * p){

  int s = p -> row;
  
  ostringstream oss;
  oss << p -> puzzle[0];
  for (int i = 1; i < s; i++)
    oss << " " << p -> puzzle[i];
  return oss.str();

}

typedef struct _puzzle_meta{

  puzzle * p;
  int big_fail;
  int big_success;
  int small_fail;
  int small_success;
  
} puzzle_meta;

map<string, puzzle_meta *> * table[MAX_K+1];

puzzle_meta * create_puzzle_meta(puzzle * p){

  puzzle_meta * pm = (puzzle_meta *)malloc(sizeof(puzzle_meta));

  pm -> p = p;
  pm -> big_fail = 0;
  pm -> big_success = 0;
  pm -> small_fail = 0;
  pm -> small_success = 0;

  return pm;
}

void destroy_puzzle_meta(puzzle_meta * pm){

  if (pm != NULL) {
    destroy_puzzle(pm -> p);
    pm -> p = NULL;
    free(pm);
  }

}

puzzle_meta * random_table_usp(int k){

  int ix = lrand48() % (table[k] -> size());
  // XXX - Not very efficient random lookup.
  map<string, puzzle_meta *>::const_iterator iter = table[k] -> begin();
  for (int i = 0; i < ix; i++){
    iter++;
  }
  return iter->second;
  
}

void construct_table_usp(puzzle * p, int iter){

  int s = p -> row;
  int k = p -> column;
  
  if (s <= 2) {
    random_usp(p);
  } else {
    int s1, s2, k1, k2;
    if (k == 3) {
      s1 = 2;
      k1 = 2;
      s2 = 1;
      k2 = 1;
    } else if (k == 4){
      s1 = 3;
      k1 = 3;
      s2 = s - s1;
      k2 = 2;
    } else if (k == 5){
      s1 = 5;
      k1 = 4;
      s2 = s - s1;
      k2 = 3;
    } else if (k == 6){
      s1 = 8;
      k1 = 5;
      s2 = s - s1;
      k2 = 4;
    } else if (k == 7){
      s1 = 13;
      k1 = 6;
      s2 = s - s1;
      k2 = 5;
    } else {
      assert(k <= 7);
    }
    

    bool found = false;
    int tries = 0;
    while (!found) {
      tries++;

      puzzle_meta * pm1 = random_table_usp(k1);
      puzzle_meta * pm2 = random_table_usp(k2);
      puzzle * p1 = pm1 -> p;
      puzzle * p2 = pm2 -> p;

      // XXX - Full twist and random twist with many iterations do not
      // produce the same results because because full twist stops
      // when it finds the first working element.
      // found = full_twist(p, p1, p2);
      found = random_twist(p, p1, p2, iter);

      if (found){
	pm1 -> big_success++;
	pm2 -> small_success++;
      } else {
	pm1 -> big_fail++;
	pm2 -> small_fail++;
      }


    }
    
  }

}


void populate_table(int s, int k, int count, int iter){

  if (table[k] == NULL)
    table[k] = new map<string, puzzle_meta *>();

  fprintf(stderr,"\r%d / %d",0,count);
  fflush(stderr);
  
  for (int i = 0; i < count; i++){
    puzzle * p = create_puzzle(s, k);
    randomize_puzzle(p);
    construct_table_usp(p, iter);
    puzzle_meta * pm = create_puzzle_meta(p);
    string p_str = puzzle_to_string(p);
    if (k >= 7)
      cerr << "  $ " << p_str << endl;
    map<string, puzzle_meta *>::const_iterator it = table[k] -> find(p_str);
    if (it == table[k] -> end()){
      // Not in table.
      table[k] -> insert(pair<string, puzzle_meta *>(p_str, pm));
    } else {
      destroy_puzzle_meta(pm);
    }

    fprintf(stderr,"\r%d / %d",i+1,count);
    fflush(stderr);
    if (k > 5) {
      print_puzzle(p);
      printf("\n");
    }
  }

}

void print_graph(map<string, puzzle_meta *> * t1, map<string, puzzle_meta *> * t2, int k, int s, int init_n){

  int n1 = t1 -> size();
  int n2 = t2 -> size();
  puzzle * p = create_puzzle(s, k);

  map<string, puzzle_meta *>::const_iterator it1;
  map<string, puzzle_meta *>::const_iterator it2;
  int ix2 = init_n + n1;
  for (it2 = t2 -> begin(); it2 != t2 -> end(); it2++){
    fprintf(stderr,"\r%4d / %4d", ix2 - n1 - init_n, n2);
    int ix1 = init_n + 0;

    for (it1 = t1 -> begin(); it1 != t1 -> end(); it1++){
      //if (random_twist(p, it2 -> second -> p, it1 -> second -> p, POP_ITER)) {
      //      bool r_res = random_twist(p, it2 -> second -> p, it1 -> second -> p, POP_ITER);
      bool f_res = full_twist(p, it2 -> second -> p, it1 -> second -> p);
      //      assert(r_res <= f_res);
      if (f_res){
	printf("%d;%d\n",ix1,ix2);
      }
      ix1++;
    }
    
    ix2++;
  }

  for (int i = init_n; i <= init_n + n1 + n2; i++){
    printf("%d;%d\n",i,i); // XXX - will duplicate sometimes.
  }
  
  destroy_puzzle(p);
  fprintf(stderr,"\rGraph print completed.\n");
}


void print_outliers(int k){

  double sig_factor = 2.0;
  
  printf("--- k = %d ---\n",k);

  double big_total = 0;
  double small_total = 0;
  map<string, puzzle_meta *>::const_iterator it;
  for (it = table[k] -> begin(); it != table[k] -> end(); it++){
    puzzle_meta * pm = it -> second;
    if (pm -> big_fail + pm -> big_success > 0)
      big_total += pm -> big_success / (double) (pm -> big_success + pm -> big_fail);
    if (pm -> small_fail + pm -> small_success > 0)
      small_total += pm -> small_success / (double) (pm -> small_success + pm -> small_fail);
  }

  int n = table[k] -> size();
  assert(n != 0);
  
  double big_avg = big_total / (double)n;
  double small_avg = small_total / (double)n;
  
  printf("big_avg = %f\n",big_avg);
  printf("small_avg = %f\n",small_avg);
  
  for (it = table[k] -> begin(); it != table[k] -> end(); it++){
    puzzle_meta * pm = it -> second;
    double curr_big_avg = -999;
    double curr_small_avg = -999;
    if (big_total == 0.0) {
      print_puzzle(pm -> p);
      printf("\n");
    } else {
      if (pm -> big_fail + pm -> big_success > 0)
	curr_big_avg = pm -> big_success / (double)(pm -> big_success + pm -> big_fail);
      
      if (pm -> small_fail + pm -> small_success > 0) 
	curr_small_avg = pm -> small_success / (double)(pm -> small_success + pm -> small_fail);
      
      if (curr_big_avg >= sig_factor * big_avg || curr_small_avg >= sig_factor * small_avg || k <= 3){
	print_puzzle(pm -> p);
	printf("^^Big Success: %6d  Fails: %6d  Avg: %8.5f\n", pm -> big_success, pm -> big_fail,     curr_big_avg);
	printf("Small Success: %6d  Fails: %6d  Avg: %8.5f\n\n", pm -> small_success, pm -> small_fail, curr_small_avg);
      }
    }
  }
  fflush(stdout);
  
}



void populate(int iter, int max_k, bool display){

  for (int k = 1; k <= 7 && k <= max_k; k++){
    populate_table(ss[k], k, ns[k], iter);
    fprintf(stderr,"\rCreated ss[%d] = %lu USPs (tried: %d)\n", k, table[k] -> size(), ns[k]);
    if (display && k >= 3)
      print_outliers(k - 2);
    if (display && (k == max_k || k == 7)){
      print_outliers(k - 1);
      print_outliers(k);
    }
    
  }
}

int main(int argc, char * argv[]){

  if (argc != 6 && argc != 3 && argc != 2 && argc != 1) {
    fprintf(stderr, "usp_construct [<k> | <file1> <file2> | <s1> <k1> <s2> <k2> <iter>]\n");
    return -1;
  }
  
  if (argc == 1){

    // Mode 1: Randomly builds a table of strong USPs up to k = 6,
    // based on the twist constants and the sizes in ns.  Displays
    // puzzles which are "useful" to stdout with some statistics.
    // Uses randomized twisting.
    
    srand48(time(NULL));
    populate(POP_ITER, 6, true);


  } else if (argc == 2){

    // Mode 2: Randomly builds a table of strong USPs up to k =
    // atoi(argv[1]), based on the twist constants and the sizes in
    // ns.  Displays a graph (as adjency list CSV to stdout) of USPs
    // found for k-1, k-2, k-3 with edges between those that can be
    // (deterministically) twisted together.
    
    int k = atoi(argv[1]);
    assert(3 <= k && k <= 6);
    populate(POP_ITER, k-1, false);

    int offset = 0;
    for (int i = 1; i <= k-2; i++){
      print_graph(table[i], table[i+1], i+2, ss[i+2], offset);
      offset += table[i] -> size();
    }
    
  } else if (argc == 3){

    // Mode 3: Takes two files containing USPs and attempts to twist
    // them together.
    
    puzzle * p1 = create_puzzle_from_file(argv[1]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle One does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    puzzle * p2 = create_puzzle_from_file(argv[2]);
    if (p1 == NULL) {
      fprintf(stderr, "Error: Puzzle Two does not exist or is incorrectly formated.\n");
      return -1;
    }
    
    if (!check(p1 -> puzzle, p1 -> row, p1 -> column)){
      fprintf(stderr, "Warning: Puzzle One is not a strong USP.\n");
    }
    
    if (!check(p2 -> puzzle, p2 -> row, p2 -> column)){
      fprintf(stderr, "Warning: Puzzle Two is not a strong USP.\n");
    }
    
    // Make p1 the wider of the two.
    if (p1 -> column < p2 -> column){
      puzzle * tmp = p1;
      p1 = p2;
      p2 = tmp;
    }
    
    puzzle * p = create_puzzle(p1 -> row + p2 -> row, p1 -> column + 1);
    randomize_puzzle(p);
    
    if (full_twist(p,p1,p2)){
      printf("Twisting successful:\n");
      print_puzzle(p);
    } else {
      printf("No twisted puzzle found.\n");
    }
    
    destroy_puzzle(p1);
    destroy_puzzle(p2);
    
  } else if (argc == 6){

    // Mode 6: Attempts to deterministically twist randomly generated
    // USPs of the specified sizes.  Displays statistics.
    
    int s1 = atoi(argv[1]);
    int k1 = atoi(argv[2]);
    int s2 = atoi(argv[3]);
    int k2 = atoi(argv[4]);
    int iter = atoi(argv[5]);

    if (k1 < k2){
      int tmpk = k1;
      int tmps = s1;
      k1 = k2;
      s1 = s2;
      k2 = tmpk;
      s2 = tmps;
    }
    
    puzzle * p1 = create_puzzle(s1,k1);
    puzzle * p2 = create_puzzle(s2,k2);

    puzzle * p = create_puzzle(p1 -> row + p2 -> row, p1 -> column + 1);
    randomize_puzzle(p);
    
    srand48(time(NULL));
    
    int found = 0;

    for (int i = 0; i < iter; i++){

      random_usp(p1);
      random_usp(p2);

      bool ret = full_twist(p, p1, p2);
      if (ret)
	found++;
      printf("Found = %d / %d\n", found, i+1);
      
    }

    printf("Found = %d / %d\n", found, iter);

    destroy_puzzle(p1);
    destroy_puzzle(p2);
    destroy_puzzle(p);
    
  }

  return 0;
 
}
