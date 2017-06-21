#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "assert.h"

//Generate two files, USP and nonUSP that respectively contains USP and nonUSP 
int main(int argc, char * argv[]){
	FILE * f, *g;
	long index = 0;
	int givenR = 4;
	int givenC = 5;
	int i, j;
	f = fopen("USP.txt", "w");
	g = fopen("nonUSP.txt", "w");
	assert(f != NULL);
	assert(g != NULL);

	for(index; index < 3^20 -1; index++){
	puzzle * p;
	p = create_puzzle_from_index(givenR, givenC, index);

	int result = check_usp(p);
	int resultR = check_usp_recursive(p);

	if(result && resultR){

	for(i=0; i<p->row;i++){
 	 for(j=0;j<p->column;j++){
   	   fprintf(f, "%d", get_column_from_row(p->puzzle[i], j));  
 	   }
  	  fprintf(f, "\n");
	  }
	 fprintf(f, "\n");
	}
	else if(!result && ! resultR){
	for(i=0; i<p->row;i++){
   	 for(j=0;j<p->column;j++){
   	   fprintf(g, "%d", get_column_from_row(p->puzzle[i], j));  
 	   }
  	  fprintf(g, "\n");
	  }
	  fprintf(g, "\n");
	 }
  	destroy_puzzle(p);
        }
  	
	
	
	fclose(f);
	fclose(g);

//check_all_usp(4, 4, 1);
}


