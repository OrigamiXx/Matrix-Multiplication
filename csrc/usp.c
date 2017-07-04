// check usp in c
// author Jerry
//add -std=c++11 back if want the original one working
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "usp.h"
#include <string>
#include "constants.h"
#include "assert.h"
#include <math.h>
#include "matrix.h"
#include "puzzle.h"
#include "set.h"
#include <map>
#include <iostream>
using namespace std;
//check the number of the usp of puzzles with same width and different height

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


//give a size of the puzzle check all the puzzles from 1X1 to this size
// whether they are USPs and
// will write out in new text files for USPs and nonUSPs if returnP is set to 1.
int check_all_usp(int row, int column, int returnP){
  int i, j, k;
  int max_index;
  int a, tt=0, tf=0, ft = 0, ff = 0;

    //printf("%d\n", max_index);
  for(i=1; i<=row; i++){
    for(j=1; j<=column; j++){
      //printf("%d\n", j);
      max_index =1;
      for(a=0;a<i*j;a++){
	max_index = max_index*3;
      }
      for(k=0; k<max_index; k++){

	puzzle * p = create_puzzle_from_index(i, j, k);
	int res_check = check_usp_recursive(p);//check_usp(p);
	int res_mult = check_usp(p); //  check_usp_mult(p);

	if(res_check && res_mult){
	 tt++;
	 //write_puzzle(p, k);
	}
	if(!res_check && !res_mult){
	  ff++;
	}
	if(!res_check && res_mult){
	  ft++;
	}
	if(res_check && !res_mult){
	  tf++;
	}

	destroy_puzzle(p);
      }
      printf("rows = %d cols = %d tt = %d ff = %d tf = %d ft = %d\n",i,j, tt, ff, tf, ft);
      assert(tf == 0 && ft == 0);
    }
  }
  return 0;
}
//get_column_from_row (row index, column index)

//check one case of pi1 pi2 pi3 holds the strong usp property
int check_usp_rows(int row1, int row2, int row3, puzzle * p){
  int i;
  for (i = 0;i< p->column;i++ ){

    if( (get_column_from_row(p->puzzle[row1], i) == 1) &&
	(get_column_from_row(p->puzzle[row2], i) == 2) &&
	(get_column_from_row(p->puzzle[row3], i) != 3) ){
       return true;
    }else if((get_column_from_row(p->puzzle[row1], i) != 1) &&
	     (get_column_from_row(p->puzzle[row2], i) == 2) &&
	     (get_column_from_row(p->puzzle[row3], i) == 3) ){
       return true;
    }else if ((get_column_from_row(p->puzzle[row1], i) == 1) &&
	      (get_column_from_row(p->puzzle[row2], i) != 2) &&
	      (get_column_from_row(p->puzzle[row3], i) == 3) ){
       return true;
     }
   }
  return false;

}
int check_usp(puzzle * p){
  perm * pi_1, * pi_2, * pi_3;
  int u, result;
  result = false;
  int row_result[20][20][20];
  int i, j, k;
  for(i = 0; i < p->row; i++){
    for(j = 0; j < p->row; j++){
      for(k = 0; k < p->row; k++){
	row_result[i][j][k] = check_usp_rows(i, j, k, p);
      }
    }
  }
  //printf("Starting check_usp\n");
  //for (pi_1 = create_perm_identity(p->row); ; pi_1 = next_perm(pi_1)){
  pi_1 = create_perm_identity(p->row);
  //printf("pi_1 = \n");
  //print_compact_perm(pi_1);
  //printf("\n");
  for (pi_2 = create_perm_identity(p->row); ; pi_2 = next_perm(pi_2)){
    //printf("pi_2 = \n");
    //print_compact_perm(pi_2);
    //printf("\n");
    for (pi_3 = create_perm_identity(p->row); ; pi_3 = next_perm(pi_3)){
      //printf("pi_3 = \n");
      //print_compact_perm(pi_3);
      //printf("\n");
      if (!(is_equals_perm(pi_1,pi_2) && is_equals_perm(pi_2, pi_3))){

	result = false;

	for (u = 0;u<p->row && !result;u++){
	  int a, b, c;
	  a= apply_perm(pi_1, u);
	  b= apply_perm(pi_2, u);
	  c= apply_perm(pi_3, u);
	  result = row_result[a][b][c];
	}

	if (!result){
	  return false;
	}
      }

      if(is_last_perm(pi_3)){
	break;
	}
    }
    destroy_perm(pi_3);
    if(is_last_perm(pi_2)){
      break;
    }
  }
  destroy_perm(pi_2);
  //if(is_last_perm(pi_1)){
  //break;
    // }
    //}
  destroy_perm(pi_1);
  return true;
}


//Return false if P is not a strong USP
int check_usp_recursive(puzzle * p){
  int row_result[20][20][20];
  int i, j, k;
  for(i = 0; i < p->row; i++){
    for(j = 0; j < p->row; j++){
      for(k = 0; k < p->row; k++){
	row_result[i][j][k] = check_usp_rows(i, j, k, p);
      }
    }
  }
  set s2 = create_empty_set();
  set s3 = create_empty_set();
//int value_not_tf = -1;
  //memo_table memo;
  /* int M[MAX_ROW][MAX_SET][MAX_SET][2];
  for(i = 0; i<MAX_ROW;i++){
    for(j=0; j<MAX_SET;j++){
      for(k=0; k<MAX_SET;k++){

	M[i][j][k][true] = value_not_tf;
	M[i][j][k][false] = value_not_tf;
      }
    }
    }*/
  //memo->M = M;
  //memo->same_perm = true;
std::map<string, bool>m;

  return !find_witness(p,0, s2, s3, row_result, true, m);
}
//turn the s2, s3, and same_perm into a string
string param_to_string(set s2, set s3, bool same_perm){
  #ifdef Cplusplus11
  string result = to_string(s2) + "|" + to_string(s3) + "|" + to_string(same_perm);
  #else
  string result = "INVALID WITHOUT C++11";
  #endif
  return result;

}

//Returns true iff there is witness for non uspness that avoids s1, s2
bool find_witness(puzzle * p, int i1, set s2, set s3, int RR[20][20] [20], bool same_perm, std::map<string, bool>&m){
  //if solved return the contain in the table
if (m.find(param_to_string(s2,s3,same_perm))!=m.end()){ //&& memo_table->same_perm == same_perm){
return m.find(param_to_string(s2,s3,same_perm))->second;
  }
  //Base step
  if (i1 == p->row && same_perm){
m.insert(pair<string, bool>(param_to_string(s2,s3,same_perm), false));
    return false;
  }
  if (i1 == p->row){
//M[i1][s2][s3][same_perm] = true;
m.insert(pair<string, bool>(param_to_string(s2,s3,same_perm), true));
    return true;
  }
  //recursive case
  int i2, i3, same_perm_new;
  set s2_new, s3_new;
  //int M_new;
  for(i2 = 0; i2 < p->row; i2++){
    if(is_membership(i2,s2)){
      continue;
    }
    for(i3 = 0; i3 < p->row; i3++){
      if(is_membership(i3,s3)){
	continue;
      }
      if (RR[i1][i2][i3]){
	continue;
      }

      if (!(i1 == i2 && i2 == i3)){
	same_perm_new = false;
      }else{
	same_perm_new = same_perm;
      }
      s2_new = set_union(s2,create_one_element_set(i2));
      s3_new = set_union(s3,create_one_element_set(i3));
      if(find_witness(p,i1+1,s2_new,s3_new,RR, same_perm_new, m)){
//M[i1][s2][s3][same_perm_new] = true;
m.insert(pair<string, bool>(param_to_string(s2,s3,same_perm), true));
	return true;
      }
    }
  }
//M[i1][s2][s3][same_perm] = false;
  m.insert(pair<string, bool>(param_to_string(s2,s3,same_perm), false));
  return false;
}
