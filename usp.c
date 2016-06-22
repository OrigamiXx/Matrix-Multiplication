// check usp in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "usp.h"
#include <string.h>
#include "constants.h"
#include "assert.h"
#include <math.h>
#include "matrix.h"
#include "puzzle.h"


//give a size of the puzzle check all the puzzles from 1X1 to this size
// whether they are USPs and
// will write out in new text file the ones that are USPs.
int check_all_usp(int row, int column){
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
	int res_check = check_usp(p);
	int res_mult = res_check; //  check_usp_mult(p);

	if(res_check && res_mult){
	 tt++;
	  write_puzzle(p, k);  
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

//check one case of pi1 pi2 pi3 holds the strong usp property
int check_usp_rows(int row1, int row2, int row3, puzzle * p){
  int i;
  for (i = 0;i< p->column;i++ ){
       
     if( (p->puzzle[row1][i] == 1) &&
	 (p->puzzle[row2][i] == 2) &&
	 (p->puzzle[row3][i] != 3) ){
       return true;
     }else if((p->puzzle[row1][i] != 1) &&
	      (p->puzzle[row2][i] == 2) &&
	      (p->puzzle[row3][i] == 3) ){
       return true;
     }else if ((p->puzzle[row1][i] == 1) &&
	       (p->puzzle[row2][i] != 2) &&
	       (p->puzzle[row3][i] == 3) ){
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
