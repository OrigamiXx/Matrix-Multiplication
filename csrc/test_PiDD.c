#include "PiDD.hpp"
 
using namespace std;

#include <cstdio> 
#include <string>
#include <cstdlib>
#include "puzzle.h"

#define MIN(a,b)  ((a) < (b) ? (a) : (b))

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


PiDD all_automorphisms(int c1, int c2, int c3){

  int s1 = 0;
  int e1 = s1 + c1 - 1;
  int s2 = e1 + 1;
  int e2 = s2 + c2 - 1;
  int s3 = e2 + 1;
  int e3 = s3 + c3 - 1;
  printf("(%d %d) (%d %d) (%d %d)\n", s1, e1, s2, e2, s3, e3);

  int n = c1 + c2 + c3;
  
  PiDD dd1 = all_perms(s1, e1);
  PiDD dd2 = all_perms(s2, e2);
  PiDD dd3 = all_perms(s3, e3);
  
  PiDD dd4 = all_perms(s1+n, e1+n);
  PiDD dd5 = all_perms(s2+n, e2+n);
  PiDD dd6 = all_perms(s3+n, e3+n);
  
  return (dd1 * dd2 * dd3) * (dd4 * dd5 * dd6);
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
   9,   // 211
   10,  // 212
   11,  // 213
   12,  // 221
   13,  // 222 
   //14,  // 223 X
   15,  // 231
   16,  // 232
   17,  // 233
   18,  // 311
   19,  // 312
   20,  // 313
   21,  // 321
   22,  // 322
   //23,  // 323 X
   24,  // 331
   25,  // 332
   26   // 333
  };
   

PiDD bad_perm_helper(int abc_ix, int cs[9], int s, int s_left, bool &valid){

  //printf("Start bad_perm_helper %d\n", s_left);
  
  PiDD res = PiDD_Factory::make_identity();
  valid = true;

  
  if (abc_ix >= 21){
    if (s_left != 0)
      valid = false;
  } else {
    int abc = abcs[abc_ix];
    if ((abc >= 3 * 3 && cs[0] != 0) || (abc >= 3 * 3 * 2 && (cs[0] != 0 || cs[3] != 0))) {
      valid = false;
    } else {
      
      bool found_something = false;
      
      int c = abc % 3;
      int b = (int)((abc - c) / 3) % 3;
      int a = (int)(abc / 9);
      
      //printf("a, b, c = %d, %d, %d\n", a, b, c);
      //printf("cs = {%d %d %d | %d %d %d | %d %d %d}\n",cs[0], cs[1],cs[2], cs[3],cs[4],cs[5],cs[6],cs[7],cs[8]);
      int max_count = MIN(MIN(cs[a*3 + 0], cs[b*3 + 1]), cs[c*3 + 2]);
      
      //printf("max_count = %d\n", max_count);
      
      for (int m = 0; m <= max_count; m++){
	
	
	int cs2[9];
	memcpy(cs2, cs, sizeof(int) * 9);
	cs2[a*3 + 0] -= m;
	cs2[b*3 + 1] -= m;
	cs2[c*3 + 2] -= m;
	bool valid2 = true;
	
	PiDD dd = bad_perm_helper(abc_ix + 1, cs2, s, s_left - m, valid2);

	if (valid2){
	  
	  int l[2] = {b, c};
	  int perm_list[2*s];
	  
	  for (int i = 0; i < 2 * s; i++){
	    perm_list[i] = i;
	  }
	  
	  for (int e = 2; e <=3; e++){
	    int j = e - 2;
	    if (l[j] + 1 == 2  || l[j] + 1 == 3){
	      int start = s - s_left;
	      int end = start + cs[0 + (j+1)] + (l[j] + 1 == 2 ? 0 : cs[1*3 + (j+1)]);
	      int shift = j * s;
	      
	      for (int i = start; i < end; i++)
		perm_list[i + shift] = i + m + shift;
	      
	      for (int i = end; i < end + m; i++)
		perm_list[i + shift] = start + (i - end) + shift;
	    }
	  }
	  
	  /* for (int i = 0; i < 2 * s; i++){ */
	  /*   printf("%d ", perm_list[i]); */
	  /* } */
	  /* printf("\n"); */
	  
	  perm * pi = create_perm(perm_list, 2*s);
	  //dd.print_perms();
	  PiDD dd2 = PiDD_Factory::make_singleton(pi);
	  //dd2.print_perms();
	  dd2 = dd2 * dd;
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
  
  //printf("End bad_perm_helper\n");
  
  return res;
}

PiDD bad_perms(puzzle * p){

  assert(p -> k == 1); // XXX - only works for columns.

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

  printf("Construct PiDDs for sorting\n");
  perm * sort_perm = create_perm(sort_perm_list, 2*s);
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
  PiDD automorphisms = all_automorphisms(c[0], c[1], c[2]);
  //automorphisms.print_perms();
  
  int cs[9] = {c[0], c[0], c[0], c[1], c[1], c[1], c[2], c[2], c[2]};
  int abc_ix = 0;

  // XXX - do memo.
  bool valid = true;

  printf("Compute all bad perms\n");
  PiDD res = bad_perm_helper(abc_ix, cs, s, s, valid);
  assert(valid);

  printf("Combine results\n");
  return (sort_inv_dd * res) * (automorphisms * sort_dd);
  
}

int main(int argc, char * argv[]){ 
   
           
  /* int n = 13;                */
  /* printf("PiDD Tester\n");             */
   
  /* printf("Empty\n");       */
  /* PiDD dd = PiDD_Factory::make_empty();         */
  /* dd.print_perms(); */
  
  /* printf("Identity\n"); */
  /* perm * pi = create_perm_identity(n); */
  /* PiDD dd2 = PiDD_Factory::make_singleton(pi); */
  /* dd2.print_perms(); */
  
  /* printf("Empty | Identity\n"); */
  /* PiDD dd3 = dd | dd2; */
  /* dd3.print_perms(); */

  /* printf("Empty & Identity\n"); */
  /* PiDD dd4 = dd & dd2; */ 
  /* dd4.print_perms(); */
 
  /* next_perm(pi);  */
 
  /* printf("First transpose\n"); */
  /* PiDD dd5 = PiDD_Factory::make_singleton(pi); */
  /* dd5.print_perms();   */

  /* printf("Identity | First Transpose\n"); */
  /* PiDD dd6 = dd2 | dd5; */
  /* dd6.print_perms();  */

  /* printf("Identity & First Transpose\n");  */
  /* PiDD dd7 = dd2 & dd5; */
  /* dd7.print_perms();  */
 
  /* printf("Apply transpose to identity\n"); */
  /* PiDD dd9 = dd2 * (transpose){1,2};  */
  /* dd9.print_perms(); */
  /* printf("Apply transpose again\n"); */
  /* PiDD dd10= dd9 * (transpose){1,2}; */
  /* dd10.print_perms(); */
  
  /* printf("PiDD count = %d\n", PiDD_count);   */
  
  /* pi = create_perm_identity(n);  */
  /* int count = 0; */
  /* while (true){ */
  /*   //print_perm_cycle(pi);  */
  /*   count++; */
  /*   PiDD dd8 = PiDD_Factory::make_singleton(pi); */
  /*   dd = dd | dd8; */
  /*   //printf("count = %d\n", count); */
  /*   //printf("size(dd) = %u\n", dd.size()); */
  /*   /\* printf("Cache size = %u\n", PiDD_Factory::size()); *\/ */
  /*   /\* printf("PiDD count = %d\n", PiDD_count); *\/ */
  /*   if (is_last_perm(pi))  */
  /*     break; */
  /*   next_perm(pi); */
  /*   if (count % 1000 == 0) { */
  /*     printf("\rcount = %d, cache = %u", count, PiDD_Factory::size()); */
  /*     fflush(stdout);  */
  /*   } */
  /* }      */
  
  /* dd = dd & dd; */
  /* printf("self intersect, size(dd) = %u\n", dd.size());  */
  /* printf("Cache sizes = %u\n", PiDD_Factory::size()); */
  /* printf("PiDD count = %d\n", PiDD_count);     */    

  //dd = all_perms(0, n-1);
  /* dd = all_automorphisms(15, 3, 2); */

  /* printf("all perms, size(dd) = %lu\n", dd.size());     */
  /* printf("Cache size = %lu\n", PiDD_Factory::size()); */
  /* printf("PiDD count = %d\n", PiDD_count);      */     

  /*
  // Permutation Networks Example from [Minato11].
  PiDD id = PiDD_Factory::make_identity();
  PiDD P = id;
  PiDD T = id;
  for (int a = 1; a < n; a++){
    int b = a - 1;
    T = T | (id * (transpose){(uint8_t) a, (uint8_t) b});
  }

  T.print_perms();
  
  for (int i = 0; i <= n * (n - 1) / 2; i++){
    P = P * T;
    printf("i = %d, cache size = %lu\n", i, PiDD_Factory::size()); 
  }

  printf("Permutation Network, size(dd) = %lu\n", P.size());   
  printf("Cache size = %lu\n", PiDD_Factory::size()); 
  printf("PiDD count = %d\n", PiDD_count);
  */

  // XXX - This puzzle is not a SUSP, but the intersection is identity...
  // Trace a simple example to check bad_perms.
  puzzle * p0 = create_puzzle_from_string((char *)"1\n3\n3\n");
  puzzle * p1 = create_puzzle_from_string((char *)"1\n2\n3\n");
  puzzle * p2 = create_puzzle_from_string((char *)"1\n1\n3\n");
  PiDD P0 = bad_perms(p0);
  PiDD P1 = bad_perms(p1);
  PiDD P2 = bad_perms(p2);

  P0.print_perms();
  P1.print_perms();
  P2.print_perms();

  PiDD P3 = P0 & P1 & P2;

  P3.print_perms();
  
  destroy_puzzle(p0);
  destroy_puzzle(p1);
  destroy_puzzle(p2);

  
  
  return 0;
}
