
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include <string.h>
#include "constants.h"
#include "assert.h"
#include <math.h>
#include "matrix.h"
#include "puzzle.h"

//create a puzzle that has one more row and same width as the input puzzle
// according to the given row_index
puzzle * create_puzzle_from_puzzle(puzzle * p, int row_index){
  puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  //int r;
  result->row = p->row+1;
  result->column = p->column;
  result->pi = create_perm_identity(result->row);
  result->puzzle = (int *) malloc(sizeof(int *)*result->row);
  int i;
  for(i = 0; i<p->row; i++){
    result->puzzle[i] = p->puzzle[i];
  }
  result->puzzle[result->row-1] = row_index;
  return result;
}

//read a puzzle from a file.
puzzle * create_puzzle_from_file(const char * filename){

  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  FILE * f = fopen(filename,"r");

  char buff[256];

  int element;//,r;
  //first check whether this file is able to turn into a puzzle
  int bytes_read = fscanf(f,"%s\n",buff);
  assert(bytes_read > 0);

  unsigned int width = strlen(buff);
  //printf("width %d\n", width);
  p->column = width;
  int rows = 1;
  //loop until end of file using feof(f).  Build up puzzle DS.
  //printf("line |%s|\n",buff);
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n",buff);
    assert(bytes_read > 0);
    //printf("line |%s|\n",buff);
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
  p->puzzle = (int *) malloc(sizeof(int *)*p->row);
  //for (r = 0; r < p->row; r++){
  //  p->puzzle[r] = (int *) malloc(sizeof(int *)*p->column);
  //}
  f = fopen(filename,"r");
  rows = 0;
  int row_index = 0, next_element;
  while(!feof(f)){
    bytes_read = fscanf(f,"%s\n", buff);
    assert(bytes_read > 0);
    //printf("line %s\n",buff);
    if (p->column >= 2){
      element = buff[p->column-1] - '0';
      element = element -1;
      next_element = buff[p->column-2] - '0';
      next_element = next_element-1;
      row_index = element*3 + next_element;
	for(int i = p->column-2; i>0; i--){
	  next_element = buff[i-1] - '0';
	  next_element = next_element - 1;
	  // printf("%d",element);
	  row_index = row_index*3 + next_element;
	  //p->puzzle[rows][i] = element;
	}
    }else if(p->column == 1){
      row_index = buff[p->column-1] - '0';
      row_index = row_index - 1;
    }
    p->puzzle[rows] = row_index;
    rows++;
  }
  return p;
}



puzzle * create_puzzle(int rows, int cols){

  puzzle * usp = (puzzle *) (malloc(sizeof(puzzle)));

  // Initialize dimensions of puzzle.
  usp -> row = rows;
  usp -> column = cols;
  usp -> pi = NULL;
  usp -> puzzle = (int *) (malloc(sizeof(int) * rows));

  return usp;
}




//give width(column) and height(row) and the index of all the possible puzzle in this size
//return a puzzle
//index must be 0-(3^(k*s)-1)
puzzle * create_puzzle_from_index(int row, int column, int index){
  puzzle * p = (puzzle *) (malloc(sizeof(puzzle)));

  p->row = row;
  p->column = column;
  p->pi = create_perm_identity(p->row);
  p->puzzle = (int *) malloc(sizeof(int *)*p->row);

  int num_type_rows = 1;
  int a;
  for (a = 0; a<column; a++){
    num_type_rows = num_type_rows * 3;
  }

  int i;
  int x;
  for(x = 0; x < row; x++){
    i = index % num_type_rows;
    p->puzzle[x] = i;
    index = index/num_type_rows;
  }
  return p;
}

// with a given puzzle return the index out of 0-(3^(r*c)-1)
int get_indext_from_puzzle(puzzle * p){
  int i;
  int result = p->puzzle[p->row-1];
  int max_row_index = pow(3,p->column);
  for (i= p->row-2; i>=0; i--){
    result = result * max_row_index + p->puzzle[i];
  }
  return result;
}



//return the data (1or2or3) of the row - col index
//row_index must from the puzzle
//col index must less than the column number of the puzzle(0- col-1)
// MWA: This could be done without an explicit loop, use the pow function instead.
int get_column_from_row(int row_index, int col_index){
  int i,result = 1;
  int num_elements_in_row = 3;
  for(i=0; i<col_index+1; i++){
  //Seemed to me that the result isn't used in the loop. Maybe move it out?
    result = row_index % num_elements_in_row;
    row_index = row_index/num_elements_in_row;
  }
  return result+1;
}

/*
 * Returns the result of setting the entry at c of row_index to val.
 */
int set_entry_in_row(int row_index, int c, int val) {

  int old_val = get_column_from_row(row_index, c);
  return row_index + (val - old_val) * (int)pow(3, c);
}

// print a puzzle
int print_puzzle(puzzle * p){
  if (p != NULL){
    int r,c;
    for(r = 0; r<p->row; r++){
      for(c = 0; c<p->column; c++){
	       printf("%d", get_column_from_row(p->puzzle[r], c));
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
  //string extension = ".puzz";
  //char * name[100];
  //string name;
  FILE * f;
  //snprintf(name, sizeof(name), "puzzles/%d_%d_%04d%s", p -> row, p -> column, index, ".txt");
  //printf(name);
  //string name;
  //name = "puzzles" + to_string(p->row) + "_" + to_string(p->column) + "_" + to_string(index) + extension;
  f = fopen("1", "w");
  assert(f != NULL);
  int i, j;
  for(i=0; i<p->row;i++){
    for(j=0;j<p->column;j++){
      fprintf(f, "%d", get_column_from_row(p->puzzle[i], j));
    }
    fprintf(f, "\n");
  }
  fclose(f);
}





// Deallocates a puzzle
void destroy_puzzle(puzzle * p){
  if (p -> pi != NULL)
    destroy_perm(p->pi);
  //int i;
  // for (i = 0; i < p -> row; i++){
  //   free(p -> puzzle[i]);
  //}
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

//Should this be i, j ,k instead of all i for the three? I assume that this is
//a strong USP check or some kind?
	  int num_sat = (get_column_from_row(p -> puzzle[i], l) == 1 ? 1 : 0)
	    + (get_column_from_row(p -> puzzle[i], l) == 1 ? 1 : 0)
	    + (get_column_from_row(p -> puzzle[i], l) == 1 ? 1 : 0);

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
