// check usp in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "CheckUSP.h"
#include <string.h>


// Todo read a puzzle from a file.
puzzle * create_puzzle_from_file(char * filename){

  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  FILE * f = fopen(filename,"r");
 
  char buff[256];

  int i, r, element;
  //first check whether this file is able to turn into a puzzle
  fscanf(f,"%s\n",buff);
  int width = strlen(buff);
  printf("width %d\n", width);
  p->column = width;
  int rows = 1;
  // Todo loop until end of file using feof(f).  Build up puzzle DS.
  printf("line |%s|\n",buff);
  while(!feof(f)){
    fscanf(f,"%s\n",buff);
    printf("line |%s|\n",buff);
    if (width != strlen(buff)){
      printf("this is not a puzzle since the width is not all the same\n");
      return NULL;
    } 
    for(i = 0; i <width; i++){
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
  p->pi = ID_permutation(p->row);
  p-> puzzle = (int **) malloc(sizeof(int *)*p->row);
  for (r = 0; r < p->row; r++){
    p->puzzle[r] = (int *) malloc(sizeof(int *)*p->column);
  }
  f = fopen(filename,"r");
  rows = 0;
  while(!feof(f)){
    fscanf(f,"%s\n", buff);
    //printf("line %s\n",buff);
    for(i = 0; i<p->column; i++){
      element = buff[i] - '0';
      // printf("%d",element);
      p->puzzle[rows][i] = element;
      //printf("hi\n");
    }
    rows++;
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
  usp -> pi = ID_permutation(8);
  
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


int CheckUSP(puzzle * p){
  permutation * pi_1, * pi_2, * pi_3;
  int u, i, result;
  result = -1;

  printf("Starting Check USP\n");

  /* print(ID_permutation(p->row)); */

  /* printf("\n"); */

  /* print(last_permutation(p->row)); */

  /* printf("\n"); */

  /* pi_1 = ID_permutation(p->row); */

  /* printf("!equal = %d\n",!equals(pi_1,last_permutation(p->row))); */

  int count = 0;

  for (pi_1 = ID_permutation(p->row); !equals(pi_1,last_permutation(p->row)) ; pi_1 = next_permutation(pi_1)){
    printf("count = %d\n",count);
    count++;
    //printf("pi_1 = \n");
    //print(pi_1);
    for (pi_2 = ID_permutation(p->row); !equals(pi_2,last_permutation(p->row)); pi_2 = next_permutation(pi_2)){
      //printf("pi_2 = \n");
      //print(pi_2);

      for (pi_3 = ID_permutation(p->row); !equals(pi_3,last_permutation(p->row)); pi_3 = next_permutation(pi_3)){
	//printf("pi_3 = \n");
	//print(pi_3);

	if (equals(pi_1,pi_2) && equals(pi_2, pi_3)){
	  continue;
	}
	else{
	  result = -1;
	}

	for (u = 0;u< p->row;u++){
	  for (i = 0;i< p->column;i++ ){
	    if( (p->puzzle[Apply_permutation(pi_1, u)][i] == 1) &&
		(p->puzzle[Apply_permutation(pi_2, u)][i] == 2) &&
		(p->puzzle[Apply_permutation(pi_3, u)][i] != 3) ){
	      result = 1;
	    }else if((p->puzzle[Apply_permutation(pi_1, u)][i] != 1) &&
		     (p->puzzle[Apply_permutation(pi_2, u)][i] == 2) &&
		     (p->puzzle[Apply_permutation(pi_3, u)][i] == 3) ){
	      result = 1;
	    }else if ((p->puzzle[Apply_permutation(pi_1, u)][i] == 1) &&
		      (p->puzzle[Apply_permutation(pi_2, u)][i] != 2) &&
		      (p->puzzle[Apply_permutation(pi_3, u)][i] == 3) ){
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
