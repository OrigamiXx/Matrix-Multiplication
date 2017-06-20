#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>

//Generate two files, USP and nonUSP that respectively contains USP and nonUSP 
int main(int argc, char * argv[]){
//	FILE * f, *g;
//	long index = 0;
//	int givenR = 4;
//	int givenC = 5;
//	int i, j;
//	f = fopen("USP.txt", "a");
//	g = fopen("nonUSP.txt", "a");
//	assert(f != NULL);
//	assert(g != NULL);
//
//	for(index; index < 3^20 -1; index++){
//	puzzle * sample;
//	sample = create_puzzle_from_index(givenR, givenC, index);
	
//	int result = check_usp(sample);

//	if(result == 1){

//	for(i=0; i<p->row;i++){
  // 	 for(j=0;j<p->column;j++){
//   	   fprintf(f, "%d", get_column_from_row(p->puzzle[i], j));  
// 	   }
//  	  fprintf(f, "\n");
//	  }
//	}
//	else{
//	for(i=0; i<p->row;i++){
//   	 for(j=0;j<p->column;j++){
//   	   fprintf(g, "%d", get_column_from_row(p->puzzle[i], j));  
// 	   }
//  	  fprintf(g, "\n");
//	  }
//	}
  	
  	
  	
//	destroy_puzzle(sample);
//	}
//	fclose(f);
//	fclose(g);
//}

check_all_usp(4, 4, 1);
