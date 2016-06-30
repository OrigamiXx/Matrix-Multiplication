
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include <string.h>
#include "constants.h"
#include "assert.h"
#include <math.h>
#include "matrix.h"
#include "puzzle.h"


//read a puzzle from a file.
puzzle * create_puzzle_from_file(char * filename){

  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  FILE * f = fopen(filename,"r");
 
  char buff[256];

  int r, element;
  //first check whether this file is able to turn into a puzzle
  int bytes_read = fscanf(f,"%s\n",buff);
  assert(bytes_read > 0);

  unsigned int width = strlen(buff);
  printf("width %d\n", width);
  p->column = width;
  int rows = 1;
  //loop until end of file using feof(f).  Build up puzzle DS.
  printf("line |%s|\n",buff);
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n",buff);
    assert(bytes_read > 0);
    printf("line |%s|\n",buff);
    if (width != strlen(buff)){
      printf("this is not a puzzle since the width is not all the same\n");
      return NULL;
    } 
    for(unsigned int i = 0; i <width; i++){
      element = buff[i] -'0';
      if(element != 1 &&  element != 2 && element != 3){
	//printf("%d", buff[i]);
	printf("this is not a puzzle since the element are not all 1 or 2 or 3\n");
	return NULL;
      }      
    }
    rows++;
  }

  //turn the file into a puzzle
  p->row = rows;
  p->pi = create_perm_identity(p->row);
  p->puzzle = (int **) malloc(sizeof(int *)*p->row);
  for (r = 0; r < p->row; r++){
    p->puzzle[r] = (int *) malloc(sizeof(int *)*p->column);
  }
  f = fopen(filename,"r");
  rows = 0;
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n", buff);
    assert(bytes_read > 0);
    //printf("line %s\n",buff);
    for(int i = 0; i<p->column; i++){
      element = buff[i] - '0';
      // printf("%d",element);
      p->puzzle[rows][i] = element;
      //printf("hi\n");
    }
    rows++;
  }
  return p;
}



puzzle * create_puzzle(int rows, int cols){

  puzzle * usp = (puzzle *) (malloc(sizeof(puzzle)));

  // Initialize dimensions of puzzle.
  usp -> row = rows;
  usp -> column = cols;
  
  // Creates a rows by cols int** array.
  usp -> puzzle = (int **) malloc(sizeof(int *)*rows);
  int r = 0;
  for (r = 0; r < rows; r++){
    usp -> puzzle[r] = (int *) malloc(sizeof(int)*cols);
  }

  // Initialize contents of puzzle....
  usp -> pi = create_perm_identity(8);
  
  //*
  usp -> puzzle[0][0] = 3;
  usp -> puzzle[0][1] = 3;
  usp -> puzzle[0][2] = 3;
  usp -> puzzle[0][3] = 3;
  usp -> puzzle[0][4] = 3;
  usp -> puzzle[0][5] = 3;
  usp -> puzzle[1][0] = 1;
  usp -> puzzle[1][1] = 3;
  usp -> puzzle[1][2] = 3;
  usp -> puzzle[1][3] = 2;
  usp -> puzzle[1][4] = 3;
  usp -> puzzle[1][5] = 3;
  usp -> puzzle[2][0] = 3;
  usp -> puzzle[2][1] = 1;
  usp -> puzzle[2][2] = 3;
  usp -> puzzle[2][3] = 3;
  usp -> puzzle[2][4] = 2;
  usp -> puzzle[2][5] = 3;
  usp -> puzzle[3][0] = 1;
  usp -> puzzle[3][1] = 1;
  usp -> puzzle[3][2] = 3;
  usp -> puzzle[3][3] = 2;
  usp -> puzzle[3][4] = 2;
  usp -> puzzle[3][5] = 3;
  usp -> puzzle[4][0] = 3;
  usp -> puzzle[4][1] = 3;
  usp -> puzzle[4][2] = 1;
  usp -> puzzle[4][3] = 3;
  usp -> puzzle[4][4] = 3;
  usp -> puzzle[4][5] = 2;
  usp -> puzzle[5][0] = 1;
  usp -> puzzle[5][1] = 3;
  usp -> puzzle[5][2] = 1;
  usp -> puzzle[5][3] = 2;
  usp -> puzzle[5][4] = 3;
  usp -> puzzle[5][5] = 2;
  usp -> puzzle[6][0] = 3;
  usp -> puzzle[6][1] = 1;
  usp -> puzzle[6][2] = 1;
  usp -> puzzle[6][3] = 3;
  usp -> puzzle[6][4] = 2;
  usp -> puzzle[6][5] = 2;
  usp -> puzzle[7][0] = 1;
  usp -> puzzle[7][1] = 1;
  usp -> puzzle[7][2] = 1;
  usp -> puzzle[7][3] = 2;
  usp -> puzzle[7][4] = 2;
  usp -> puzzle[7][5] = 2;
  //*/
   
  return usp;
}




//give width(column) and height(row) and the index of all the possible puzzle in this size
//return a puzzle
//index must be 0-(2^(k*s)-1)
puzzle * create_puzzle_from_index(int row, int column, int index){
  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));
  int r;
  p->row = row;
  p->column = column;
  p->pi = create_perm_identity(p->row);
  p->puzzle = (int **) malloc(sizeof(int *)*p->row);
  for (r = 0; r < p->row; r++){
    p->puzzle[r] = (int *) malloc(sizeof(int *)*p->column);
  }
  int num_elements_in_row = 3;
  //int num_type_rows = (int)pow(3, column); power fuction not working!
  int num_type_rows = 1;
  int a;
  for (a = 0; a<column; a++){
    num_type_rows = num_type_rows * 3;
  }
  int i, j;
  int x, y;
  for(x = 0; x < row; x++){
    i = index % num_type_rows;
    //printf("%d", i);
    for(y = 0; y < column; y++){
      j = i % num_elements_in_row;
      p->puzzle[x][y] = j+1;
      i = i/num_elements_in_row;
    }
    index = index/num_type_rows;
  }
  return p;
}


// print a puzzle
int print_puzzle(puzzle * p){
  if (p != NULL){
    
  
    int r,c;
    for(r = 0; r<p->row; r++){
      for(c = 0; c<p->column; c++){
	printf("%d", p->puzzle[r][c]);
      }
      printf("\n");
    }
  }
  else{
    printf("puzzle is a Null.\n");
  }
  return 0;
}


// Write the puzzle to a new text file.
void write_puzzle(puzzle * p, int index){
  //char a[256];
  //itoa(index, a);
  char * extension = ".puzz";
  char name[20+strlen(extension)+1];
  FILE * f;
  snprintf(name, sizeof(name), "puzzles/%d_%d_%04d%s", p -> row, p -> column, index, extension);
  f = fopen(name, "w");
  assert(f != NULL);
  int i, j;
  for(i=0; i<p->row;i++){
    for(j=0;j<p->column;j++){
      fprintf(f, "%d", p->puzzle[i][j]);  
    }
    fprintf(f, "\n");
  }
  fclose(f);
}





// Deallocates a puzzle
void destroy_puzzle(puzzle * p){
  destroy_perm(p->pi);
  int i;
  for (i = 0; i < p -> row; i++){
    free(p -> puzzle[i]);
  }
  free(p -> puzzle);
  free(p);
}


int count_witnesses(puzzle * p){

  int i,j,k,l;
  int count = 0;

  for (i = 0; i < p -> row; i++){
    for (j = 0; j < p -> row; j++){
      
      for (k = 0; k < p -> row; k++){

	for (l = 0; l < p -> column; l++){

	  int num_sat = (p -> puzzle[i][l] == 1 ? 1 : 0) 
	    + (p -> puzzle[j][l] == 1 ? 1 : 0) 
	    + (p -> puzzle[k][l] == 1 ? 1 : 0);
	  
	  if (num_sat == 2) {
	    count++;
	    break;
	  }
	  
	}
      }
    }
  }

  return count;

}
