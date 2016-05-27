// check usp in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

typedef struct P {
  permutation pi;
  int ** puzzle;
  int row;
  int column;
}P;

P * usp = (permutation *) malloc(sizeof(P));
usp -> puzzle = malloc(sizeof(sizeof(int)*3)*6);
usp -> puzzle[6][3] = {
  {1,2,1},
  {1,2,2},
  {1,1,3},
  {1,3,3},
  {2,2,3},
  {3,2,3}
};
usp -> row = 6;
usp -> column =3;
//usp -> pi = ID_permutaion(6);

int CheckUSP(P p){
  permutation pi_1, pi_2, pi_3;
  int u, i, result;
  result = -1;

  for (pi_1 = ID_permutation(p->row); pi_1!=last_permutation(ID_permutation(p->row)); pi_1 = next_permutation(pi_1)){
    for (pi_2 = ID_permutation(p->row); pi_2!=last_permutation(ID_permutation(p->row)); pi_2 = next_permutation(pi_2)){
      for (pi_3 = ID_permutation(p->row); pi_3!=last_permutation(ID_permutation(p->row)); pi_3 = next_permutation(pi_3)){
	if (pi_1 == pi_2 && pi_2 == pi_3){
	  countinue;
	}
	else{
	  result = -1;
	}
	for (u = 0;u< p->row;u++){
	  for (i = 0;i< p->column;i++ ){
	    if( (p->puzzle[Apply_permutation(pi_1, u)][i] = 1) &&
		(p->puzzle[Apply_permutation(pi_2, u)][i] = 2) &&
		(p->puzzle[Applu_permutation(pi_3, u)][i] != 3) ){
	      result = 1;
	    }else if((p->puzzle[Apply_permutation(pi_1, u)][i] != 1) &&
		(p->puzzle[Apply_permutation(pi_2, u)][i] = 2) &&
		(p->puzzle[Applu_permutation(pi_3, u)][i] = 3) ){
	      result = 1;
	    }else if (p->puzzle[Apply_permutation(pi_1, u)][i] = 1) &&
		(p->puzzle[Apply_permutation(pi_2, u)][i] != 2) &&
		(p->puzzle[Applu_permutation(pi_3, u)][i] = 3) ){
	      result = 1;
	    }   
	  }
	}
      if (result == -1){
	return -1;
      }

      }
    }
  }
  return 1;
}
