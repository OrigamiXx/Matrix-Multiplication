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
	//printf("%d\n",check_usp(p));
	int res_check = check_usp(p);
	int res_mult = res_check; //  check_usp_mult(p);

	if(res_check && res_mult){
          //print_puzzle(p);
	  //printf("----------\n");
	 tt++;
	  write_puzzle(p, k);  
	}
	if(!res_check && !res_mult){
	  ff++;
	  //print_puzzle(p);
	}
	if(!res_check && res_mult){
	  ft++;
	  //print_puzzle(p);
	}
	if(res_check && !res_mult){
	  tf++;
	  //print_puzzle(p);
	}
	
	destroy_puzzle(p);
      }
      printf("rows = %d cols = %d tt = %d ff = %d tf = %d ft = %d\n",i,j, tt, ff, tf, ft);
      assert(tf == 0 && ft == 0);
    }
  }



  return 0;
}




int check_usp(puzzle * p){
  perm * pi_1, * pi_2, * pi_3;
  int u, i, result;
  result = false;

  //printf("Starting check_usp\n");

  /* print(create_perm_identity(p->row)); */

  /* printf("\n"); */

  /* print(last_perm(p->row)); */

  /* printf("\n"); */

  /* pi_1 = create_perm_identity(p->row); */

  /* printf("!equal = %d\n",!is_equals_perm(pi_1,last_perm(p->row))); */

  int count = 0;
  for (pi_1 = create_perm_identity(p->row); ; pi_1 = next_perm(pi_1)){
    //printf("count = %d\n",count);
    count++;
    //printf("%d\n", is_last_perm(pi_1));
    //printf("pi_1 = \n");
    //print_compact_perm(pi_1);
    //printf("\n");
    for (pi_2 = create_perm_identity(p->row); ; pi_2 = next_perm(pi_2)){
      //printf("pi_2 = \n");
      //print_compact_perm(pi_2);
      //printf("\n");
      for (pi_3 = create_perm_identity(p->row); ; pi_3 = next_perm(pi_3)){
	//	printf("pi_3 = \n");
	//print_compact_perm(pi_3);
	//printf("\n");
	if (!(is_equals_perm(pi_1,pi_2) && is_equals_perm(pi_2, pi_3))){

	  result = false;

	  for (u = 0;u< p->row;u++){
	    //printf("u = %d\n",u);
	    for (i = 0;i< p->column;i++ ){
	      //printf("pi1(u) = %d\n", apply_perm(pi_1, u));
	      //printf("pi2(u) = %d\n", apply_perm(pi_2, u));
	      //printf("pi3(u) = %d\n", apply_perm(pi_3, u));
	      if( (p->puzzle[apply_perm(pi_1, u)][i] == 1) &&
		  (p->puzzle[apply_perm(pi_2, u)][i] == 2) &&
		  (p->puzzle[apply_perm(pi_3, u)][i] != 3) ){
		result = true;
	      }else if((p->puzzle[apply_perm(pi_1, u)][i] != 1) &&
		       (p->puzzle[apply_perm(pi_2, u)][i] == 2) &&
		       (p->puzzle[apply_perm(pi_3, u)][i] == 3) ){
		result = true;
	      }else if ((p->puzzle[apply_perm(pi_1, u)][i] == 1) &&
			(p->puzzle[apply_perm(pi_2, u)][i] != 2) &&
			(p->puzzle[apply_perm(pi_3, u)][i] == 3) ){
		result = true;
	      }
	      //printf("result = %d\n",result);
	    }
	  }
	  
	  if (!result){
	    return false;
	  }

	}

	//printf("break?\n");

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
    if(is_last_perm(pi_1)){
      break;
    }
  }

  destroy_perm(pi_1);

  return true;
}
