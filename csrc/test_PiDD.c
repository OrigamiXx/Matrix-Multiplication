#include "PiDD.hpp"
 
using namespace std;

#include <cstdio> 
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "puzzle.h"
#include "checker.h"

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define ACCESS(e, c) (3*(e) + (c))

// Returns PiDD of all perms over indexes start to end.
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

PiDD all_perms_coupled(int start, int end, int n){
  
  PiDD id = PiDD_Factory::make_identity();  

  PiDD res = id;
  
  for (int a = start + 1; a <= end; a++){    
    for (int b = a - 1; b >= start; b--){
      PiDD curr = id | (id * (transpose){(uint8_t)a, (uint8_t)b} * (transpose){(uint8_t)(a + n), (uint8_t)(b+n)});
      res = res * curr;   
    }  
  }
  
  return res;

}


PiDD all_automorphisms(int c1, int c2, int c3, bool coupled){
 
  int s1 = 0;
  int e1 = s1 + c1 - 1;
  int s2 = e1 + 1;
  int e2 = s2 + c2 - 1;
  int s3 = e2 + 1;
  int e3 = s3 + c3 - 1;
  //printf("(%d %d) (%d %d) (%d %d)\n", s1, e1, s2, e2, s3, e3);

  int n = c1 + c2 + c3;

  if (!coupled){
    PiDD dd1 = all_perms(s1, e1);
    PiDD dd2 = all_perms(s2, e2);
    PiDD dd3 = all_perms(s3, e3);
    
    PiDD dd4 = all_perms(s1+n, e1+n);
    PiDD dd5 = all_perms(s2+n, e2+n);
    PiDD dd6 = all_perms(s3+n, e3+n);
  
    return (dd1 * dd2 * dd3) * (dd4 * dd5 * dd6);
  } else {
    
    PiDD dd1 = all_perms_coupled(s1, e1, n);
    PiDD dd2 = all_perms_coupled(s2, e2, n);
    PiDD dd3 = all_perms_coupled(s3, e3, n);
			 
    return dd1 * dd2 * dd3;
  }
  
}

perm * create_perm(int * l, int n){

  perm * pi = create_perm_identity(n);
  for (int i = 0; i < n; i++){
    pi -> arrow[i] = l[i];
  }
  return pi;
}

// Row combinations to avoid for non SUSPs.
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
   

PiDD bad_perm_helper(int abc_ix, int cs[9], int s, int s_left,
		     bool &valid, unordered_map<string, pair<PiDD,bool> > &memo){
  
  ostringstream ss;
  ss << abc_ix;
  //printf("Creating key\n");
  for (int i = 0; i < 9; i++)
    ss << "|" << cs[i];
  string key = ss.str();
  //cout << key << endl;
  auto it = memo.find(key);
  if (it != memo.end()) {
    //printf("Found!\n");    
    if (it -> second.second){
      //printf("Valid\n");
      valid = true;
      return (it -> second.first);
    } else {
      //printf("Invalid\n");
      valid = false;
      return PiDD_Factory::make_identity();
    } 
  }
  //printf("Not found\n"); 
  
   
  PiDD res = PiDD_Factory::make_identity();
  valid = true;

  if (s_left == 0){
    valid = true;
  } else if (abc_ix >= 21 && s_left != 0) {
    valid = false;
  } else {
    int abc = abcs[abc_ix];
    if ((abc_ix >= 5 && cs[ACCESS(0,0)] != 0) || (abc_ix >= 13 && (cs[ACCESS(0,0)] != 0 || cs[ACCESS(1,0)] != 0))) {
      valid = false;
    } else {
      
      bool found_something = false;
      
      int c = abc % 3;
      int b = (int)((abc - c) / 3) % 3;
      int a = (int)(abc / 9);
      
      //printf("a, b, c = %d, %d, %d\n", a, b, c);
      //printf("cs = {%d %d %d | %d %d %d | %d %d %d}\n",cs[0], cs[1],cs[2], cs[3],cs[4],cs[5],cs[6],cs[7],cs[8]);
      int max_count = MIN(MIN(cs[ACCESS(a,0)], cs[ACCESS(b,1)]), cs[ACCESS(c, 2)]);
      
      //printf("max_count = %d\n", max_count);
      
      for (int m = 0; m <= max_count; m++){
	
	
	int cs2[9];
	memcpy(cs2, cs, sizeof(int) * 9);
	cs2[ACCESS(a,0)] -= m;
	cs2[ACCESS(b,1)] -= m;
	cs2[ACCESS(c,2)] -= m;
	bool valid2 = true;
	
	PiDD dd = bad_perm_helper(abc_ix + 1, cs2, s, s_left - m, valid2, memo);

	if (valid2){

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
	  
	  /* for (int i = 0; i < 2 * s; i++){ */
	  /*   printf("%d ", perm_list[i]); */
	  /* } */
	  /* printf("\n"); */
	  
	  perm * pi = create_perm(perm_list, 2*s);
	  //dd.print_perms();
	  PiDD dd2 = PiDD_Factory::make_singleton(pi);
	  //dd2.print_perms();
	  dd2 = dd * dd2;
	  //dd.print_perms();
	  destroy_perm(pi);
	  
	  res = res | dd2;
	  found_something = true;
	  
	}	
      }
      
      if (!found_something){
	valid = false;
      }
    } 
  } 

  //printf("Start Insert: %u\n", PiDD_count);
  if (valid){
    memo.insert(make_pair(key, make_pair(res, true)));
  } else {
    memo.insert(make_pair(key, make_pair(PiDD_Factory::make_identity(), false)));
  }
  //printf("End Insert\n");
  
  return res;
}

PiDD bad_perms(puzzle * p){
  // Only works for columns.
  
  assert(p -> k == 1); 

  int s = p -> s;

  printf("Compute counts\n");
  int c[3] = {0,0,0};
  for (int r = 0; r < s; r++)
    c[get_entry(p, r, 0) - 1] += 1;

  int sort_perm_list[2*s];
  int sort_perm_inv_list[2*s];
  int c_sort[3] = {0, c[0], c[0] + c[1]};

  printf("Compute sort perms\n");
  for (int r = 0; r < s; r++){
    int entry = get_entry(p, r, 0) - 1;
    sort_perm_list[r] = c_sort[entry];
    sort_perm_list[r + s] = c_sort[entry] + s;
    sort_perm_inv_list[c_sort[entry]] = r;
    sort_perm_inv_list[c_sort[entry] + s] = r + s;
    c_sort[entry] += 1;
  }

  /* for (int r = 0; r < s; r++){ */
  /*   printf("%d ", sort_perm_list[r]); */
  /* } */
  /* printf("\n"); */

  printf("Construct PiDDs for sorting\n");
  perm * sort_perm = create_perm(sort_perm_list, 2*s);
  //print_perm_cycle(sort_perm);
  //print_perm_cycle(sort_perm);
  PiDD sort_dd = PiDD_Factory::make_singleton(sort_perm);
  destroy_perm(sort_perm);
  //sort_dd.print_perms();

  perm * sort_perm_inv = create_perm(sort_perm_inv_list, 2*s);
  //print_perm_cycle(sort_perm_inv);
  PiDD sort_inv_dd = PiDD_Factory::make_singleton(sort_perm_inv);
  destroy_perm(sort_perm_inv);
  //sort_inv_dd.print_perms();

  printf("Construct automorphisms\n");
  PiDD input_auto = all_automorphisms(c[0], c[1], c[2], false);
  uint64_t before = PiDD_Factory::size();
  PiDD output_auto = all_automorphisms(c[0], c[1], c[2], true);
  uint64_t after = PiDD_Factory::size();
  //input_auto.print_perms();
  //output_auto.print_perms();
  printf("size changed by output_auto: %ld\n", (int64_t)after - (int64_t)before);
  
  int cs[9] = {c[0], c[0], c[0], c[1], c[1], c[1], c[2], c[2], c[2]};
  int abc_ix = 0;

  bool valid = true;
  unordered_map<string, pair<PiDD,bool> > memo;

  printf("Compute all bad perms\n");
  PiDD res = bad_perm_helper(abc_ix, cs, s, s, valid, memo);
  assert(valid);

  /* for (auto element : memo) { */
  /*   if (element.second != NULL) */
  /*     delete element.second; */
  /* } */

  printf("Combine results\n"); 
  //res.print_perms();
  return (sort_inv_dd * (output_auto * (res * (input_auto * sort_dd))));  // This association seems fastest...
  
}

int main(int argc, char * argv[]){ 
   
  if (argc != 2){
    fprintf(stderr,"usage: usp_test_file <filename>\n");
    return -1;
  }

  puzzle * p = create_puzzle_from_file(argv[1]); 
  if (p == NULL) {
    fprintf(stderr,"Error: File does not exist or is not properly formated.\n");
    return -1;
  } 
  
  print_puzzle(p);
 
  printf("Initializing all automorphisms.\n");
  PiDD P = all_automorphisms(p -> s, 0, 0, false); 
  //P.print_perms();
  
  printf("Reading puzzle\n");  
  for (unsigned int c = 0; c < p -> k; c++){
    puzzle * p2 = create_puzzle(p -> s, 1);

    for (unsigned int r = 0; r < p -> s; r++){
      set_entry(p2, r, 0, get_entry(p, r, c));
    }
    printf("\n---------------New column:\n");
    print_puzzle(p2);
    PiDD P2 = bad_perms(p2);
    //P2.print_perms();
    P = P & P2;
    P.print_stats();
    destroy_puzzle(p2);
  }

  //P.print_perms();

  P.print_stats();
  
  bool usp = (check(p) == IS_USP);
  bool small_intersect = P.size() == 1;

  if (usp)
    printf("Is SUSP.\n");
  else
    printf("Not SUSP.\n");
  
  if (usp != small_intersect){
    printf("Warning! Disagrees with check(). ");
    if (usp)
      printf("Says a SUSP isn't one.\n");
    else
      printf("Says non-SUSP is one.\n");
  } else {
    printf("Consistent with check().\n");
  }

  destroy_puzzle(p);
  
  return 0;
}
