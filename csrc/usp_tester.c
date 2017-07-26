#include "permutation.h"
#include <stdio.h>
#include <stdlib.h>
#include "usp.h"
#include <math.h>
#include "constants.h"
#include <map>
#include "usp_bi.h"
#include "puzzle.h"
#include <time.h>
#include <sys/time.h>
#include "3DM_to_SAT.h"
int main(int argc, char * argv[]){

  //puzzle * p = create_puzzle(8,6); //6,3); // correct one (8,6)

  /*puzzle * p = create_puzzle_from_file("puzzles/test2.puz");
  if (p == NULL) {
    printf("File does not exist or is not properly formated.\n");
    return -1;
  }
  print_puzzle(p);

  if (check(p->puzzle, p->row, p->column)){
    printf("Strong USP.\n");
  } else {
    printf("Not a Strong USP.\n");
  }*/
  int checked = 0;
  int checked2 = 0;
  int usps = 0;
  char buff[256];
  int element;
  int next_element;
  int row;
  int row_index;
  struct timespec begin={0,0}, end={0,0};
  FILE * puzzles = fopen("data/13-by-6-big.txt", "r");
  while(!feof(puzzles)){
    puzzle * p = create_puzzle(14,6);
    row = 0;
    row_index= 0;
    while (fgets(buff,sizeof(buff),puzzles) != NULL && buff[0]!= '\n'){
      element = buff[p->column-1] - '0';
      element = element -1;
      next_element = buff[p->column-2] - '0';
      next_element = next_element-1;
      row_index = element*3 + next_element;
      for(int i = p->column-2; i>0; i--){
         next_element = buff[i-1] - '0';
         next_element = next_element - 1;
         //printf("%d",element);
         row_index = row_index*3 + next_element;
         //p->puzzle[rows][i] = element;
      }
      p->puzzle[row] = row_index;
      row++;
      //printf("got here \n");
    }
    //print_puzzle(p);
    for (int i = 0; i < 729; i++){
      p->puzzle[row] = i;
      //print_puzzle(p);
      //printf("got here \n");
      clock_gettime(CLOCK_MONOTONIC, &begin);
      if //(check(p->puzzle,p->row,p->column)){
        (check_usp_bi(p->puzzle,p->row,p->column)){
        //(solver_simple(p->row, p->column,-1,p)){
        printf("this puzzle is a new 14 by 6y usp\n");
        print_puzzle(p);
        usps++;
      }
      clock_gettime(CLOCK_MONOTONIC, &end);
      printf("this one %d takes %.5f\n", i, ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec));
    }
    destroy_puzzle(p);
    checked++;
    printf("rua%d\n",checked);
  }
  printf("checked 13by6 %d\n", checked);
  printf("checked 14by6 %d\n", checked2);
  printf("found %d\n", usps);







  //if(check_usp_recursive(p)){
  //int i = check_usp(p);
  //printf("hello%d\n", i);
  //}


  //printf("result = %d\n",check_usp(p));
  //printf("%d\n",sizeof(int));
  //printf("%d\n",sizeof(long));
  //printf("%d\n",sizeof(long long));




    //int givenR = 8;
    //int givenC = 6;

   //int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  //puzzle1[0] = 279;
  //puzzle1[1]= 284;
  //puzzle1[2]= 290;
  //puzzle1[3]= 297;
  //puzzle1[4] = 318;
  //puzzle1[5] = 350;
  //puzzle1[6] = 389;
  //puzzle1[7] = 487;
  //puzzle1[8] = 519;
  //puzzle1[9] = 586;
  //puzzle1[10] = 591;
  //puzzle1[11] = 630;
  //puzzle1[12] = 637;
  //puzzle1[13] = 642;



  //puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  //result->row = givenR;
  //result->column = givenC;
  //result->pi = create_perm_identity(result->row);
  //result -> puzzle = puzzle1;

  //print_puzzle(result);

  //topcheck_usp(result);
  //printf("%d", check_usp(result));

  //  printf("%d", check(puzzle1, givenR, givenC));

  /* puzzle * sample; */
  /* 	sample = create_puzzle_from_index(8, 6, 10); */

  /* 	print_puzzle(sample); */
  /* 	printf("%d", check_usp_recursive(sample)); */
  /* 	write_puzzle(sample, 10); */
  /* 	destroy_puzzle(sample); */

  return 0;

}
