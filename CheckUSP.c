// check usp in c
// author Jerry

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "CheckUSP.h"
#include <string.h>
#include "constants.h"
#include <math.h>
#include "matrix.h"
char* itoa(int i, char b[]){
  char const digit[] = "0123456789";
  char*p = b;
  if(i<0){
    *p++ = '-';
    i *= -1;
  }
  int shifter = i;
  do{
    ++p;
    shifter =shifter/10;
  }while(shifter);
  *p = '\0';
  do{
    *--p = digit[i%10];
    i = i/10;
  }while(i);
    return b;
}
//Deallocattes a puzzle
void destroy_puzzle(puzzle * p){
  free(p->pi);
  free(p->puzzle);
  free(p);
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

//write the puzzle to a new text file
void write_puzzle(puzzle * p, int index){
  //char a[256];
  //itoa(index, a);
  char * extension = ".puzz";
  char name[20+strlen(extension)+1];
  FILE * f;
  snprintf(name, sizeof(name), "%d_%d_%04d%s", p -> row, p -> column, index, extension);
  f = fopen(name, "w");
  int i, j;
  for(i=0; i<p->row;i++){
    for(j=0;j<p->column;j++){
      fprintf(f, "%d", p->puzzle[i][j]);  
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

//give a size of the puzzle check all the puzzles from 1X1 to this size
// whether they are USPs and
// will write out in new text file the ones that are USPs.
int check_all_usp(int row, int column){
  int i, j, k;
  int max_index;
  int a;
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
	printf("%d\n",CheckUSP(p));
	if(CheckUSP(p)){// && is_usp(p)){
          print_puzzle(p);
	  printf("----------\n");
	  write_puzzle(p, k);  
	}
	destroy_puzzle(p);
      }  
    }
  }



  return 0;
}



//read a puzzle from a file.
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
  //loop until end of file using feof(f).  Build up puzzle DS.
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
  p->pi = create_perm_identity(p->row);
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


int CheckUSP(puzzle * p){
  perm * pi_1, * pi_2, * pi_3;
  int u, i, result;
  result = -1;

  printf("Starting Check USP\n");

  /* print(create_perm_identity(p->row)); */

  /* printf("\n"); */

  /* print(last_perm(p->row)); */

  /* printf("\n"); */

  /* pi_1 = create_perm_identity(p->row); */

  /* printf("!equal = %d\n",!is_equals_perm(pi_1,last_perm(p->row))); */

  int count = 0;
  for (pi_1 = create_perm_identity(p->row); !is_last_perm(pi_1) ; pi_1 = next_perm(pi_1)){
    //printf("count = %d\n",count);
    count++;
    printf("%d\n", is_last_perm(pi_1));
    printf("pi_1 = \n");
    print_compact_perm(pi_1);
    printf("\n");
    for (pi_2 = create_perm_identity(p->row); !is_last_perm(pi_2); pi_2 = next_perm(pi_2)){
      printf("pi_2 = \n");
      print_compact_perm(pi_2);
      printf("\n");
      for (pi_3 = create_perm_identity(p->row); ; pi_3 = next_perm(pi_3)){
	printf("pi_3 = \n");
	print_compact_perm(pi_3);
	printf("\n");
	if (is_equals_perm(pi_1,pi_2) && is_equals_perm(pi_2, pi_3)){
	  continue;
	}
	else{
	  result = -1;
	}
	
	for (u = 0;u< p->row;u++){
	  for (i = 0;i< p->column;i++ ){
	    if( (p->puzzle[apply_perm(pi_1, u)][i] == 1) &&
		(p->puzzle[apply_perm(pi_2, u)][i] == 2) &&
		(p->puzzle[apply_perm(pi_3, u)][i] != 3) ){
	      result = 1;
	    }else if((p->puzzle[apply_perm(pi_1, u)][i] != 1) &&
		     (p->puzzle[apply_perm(pi_2, u)][i] == 2) &&
		     (p->puzzle[apply_perm(pi_3, u)][i] == 3) ){
	      result = 1;
	    }else if ((p->puzzle[apply_perm(pi_1, u)][i] == 1) &&
		      (p->puzzle[apply_perm(pi_2, u)][i] != 2) &&
		      (p->puzzle[apply_perm(pi_3, u)][i] == 3) ){
	      result = 1;
	    }
	  }
	}
	
	if (result == -1){
	  return false;
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
    if(is_last_perm(pi_3)){
      break;
    }
  }

  destroy_perm(pi_1);

  return true;
}
