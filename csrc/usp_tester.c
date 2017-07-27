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
#include "checkUSP_mip.h"

int main(int argc, char * argv[]){

  // distribution
  int one = 0, two=0, three=0, four=0;
  int five=0, six=0, seven=0, eight = 0;
  int checked = 0;
  int checked2 = 0;
  double total = 0;
  double individual = 0;
  int usps = 0;
  char buff[256];
  int element;
  int next_element;
  int row;
  int row_index;
  int time = 46800;

  struct timespec begin={0,0}, end={0,0};
  FILE * puzzles = fopen("data/13-by-6-data.txt", "r");
  //FILE * puzzles = fopen("1", "r");
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
    if (total>time) {
      break;
    }
    for (int i = 0; i < 729; i++){
      p->puzzle[row] = i;
      //print_puzzle(p);
      //printf("got here \n");
      clock_gettime(CLOCK_MONOTONIC, &begin);
      if //(check(p->puzzle,p->row,p->column)){
        //(check_usp_bi(p->puzzle,p->row,p->column)){
        //(check_MIP(p)){
        (solver_simple(p->row, p->column,-1,p)){
        printf("this puzzle is a new 14 by 6y usp it's rua %d and index %d\n", checked+1, i);
        //write_puzzle(p, -1);
        //print_puzzle(p);
        usps++;
      }
      clock_gettime(CLOCK_MONOTONIC, &end);
      checked2++;
      individual = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);

      total += individual;
      //printf("this one %d takes %.5f\n", i, ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec));
      if (individual>=0 && individual < 0.01){
        one++;
      } else if(individual >= 0.01 && individual < 0.1){
        two++;
      } else if (individual >= 0.1 && individual < 1){
        three++;
      } else if (individual >= 1 && individual < 10){
        four++;
      } else if (individual >= 10 && individual < 50){
        five++;
      } else if (individual >= 50 && individual < 100){
        six++;
      } else if (individual >= 100 && individual < 300){
        seven++;
      } else {
        eight++;
      }
      if (total>time) {
        break;
      }
    }
    destroy_puzzle(p);
    checked++;
    //printf("rua%d\n",checked);

  }

  finalize_check_MIP();

  printf("checked 13by6 %d\n", checked);
  printf("checked 14by6 %d\n", checked2);
  printf("found %d\n", usps);
  printf("total time %.5f and avg checking time %.5f\n",total, total/checked2);
  printf("there are: \n");
  printf("%d of 0-0.01 \n", (int) one);
  printf("%d of 0.01-0.1 \n", (int) two);
  printf("%d of 0.1-1 \n", three);
  printf("%d of 1-10 \n", four);
  printf("%d of 10-50 \n", five);
  printf("%d of 50-100 \n", six);
  printf("%d of 100-300 \n", seven);
  printf("%d of 300+ \n", eight);







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
