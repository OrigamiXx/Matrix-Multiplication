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
  FILE * usp_file, * nonusp_file;
  long index = 0;
  int givenR = 4;
  int givenC = 4;
  int i, j;
  usp_file = fopen("USP.txt", "w");
  nonusp_file = fopen("nonUSP.txt", "w");
  assert(usp_file != NULL);
  assert(nonusp_file != NULL);
  //printf("%d\n", 3^20);


  for(index; index < pow(3, givenR*givenC) -1; index+=1){
    puzzle * p;
    p = create_puzzle_from_index(givenR, givenC, index);

    int result = check_usp(p);
    //int resultR = check_usp_recursive(p);

    if(result){ //&& resultR){

      for(i=0; i<p->row;i++){
	for(j=0;j<p->column;j++){
	  fprintf(usp_file, "%d", get_column_from_row(p->puzzle[i], j));
	}
	fprintf(usp_file, "\n");
      }
      fprintf(usp_file, "\n");
    }
    else {//(!result){// && ! resultR){
      for(i=0; i<p->row;i++){
	for(j=0;j<p->column;j++){
	  fprintf(nonusp_file, "%d", get_column_from_row(p->puzzle[i], j));
	}
	fprintf(nonusp_file, "\n");
      }
      fprintf(nonusp_file, "\n");
    }
    destroy_puzzle(p);
  }

  fclose(usp_file);
  fclose(nonusp_file);

  //check_all_usp(4, 4, 1);
}
