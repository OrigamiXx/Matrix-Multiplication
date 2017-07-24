#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c++.h"
#include "puzzle.h"
#include "usp.h"
#include "checkUSP_mip.h"
#include "permutation.h"
#include "usp_bi.h"
#include <time.h>
#include <sys/time.h>


int main(int argc, char * argv[]){

  int givenR = 15;
  int givenC = 6;

  // int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  // puzzle1[0] = 279;
  // puzzle1[1]= 284;//284
  // puzzle1[2]= 290;
  // puzzle1[3]= 297;
  // puzzle1[4] = 318;
  // puzzle1[5] = 350;
  // puzzle1[6] = 389;
  // puzzle1[7] = 487;
  // puzzle1[8] = 519;
  // puzzle1[9] = 586;
  // puzzle1[10] = 591;
  // puzzle1[11] = 630;
  // puzzle1[12] = 637;
  // puzzle1[13] = 642;
  // puzzle * result = (puzzle *) (malloc(sizeof(puzzle)));
  // result->row = givenR;
  // result->column = givenC;
  // result -> puzzle = puzzle1;
  // int maxIndex = pow(3, givenR * givenC) -1;
  // int result1, result2;
  // int counter = 0;
  // double totalTime = 0.0;
  // struct timespec begin={0,0}, end={0,0};
  //  for(int index = 0; index <= maxIndex; index += 11){
  //    counter++;
  //    puzzle * p;
  //    p = create_puzzle_from_index(givenR, givenC, index);
  //
  //   //  clock_gettime(CLOCK_MONOTONIC, &begin);
  //   //  result1 = DM_to_MIP(p);
  //   //  clock_gettime(CLOCK_MONOTONIC, &end);
  //    //totalTime = totalTime + ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
  //    //printf("Current time is %d.", totalTime);
  //    result2 = check(p -> puzzle, p->row, p -> column);
  //   //  if (result1 != result2){
  //   //    printf("There are disagreement, 3DM is %d and check is %d", result1, result2);
  //   //    print_puzzle(p);
  //   //    break;
  //   //  }
  //    destroy_puzzle(p);
  //  }
  // printf("loop ended");
  // printf("Avg time is:%.6f", totalTime/counter);
  int i, j;
  //int checked = 0;
  long index;
  GRBenv   *env   = NULL;
  GRBloadenv(&env, NULL);

    i = givenC;
  //j = givenR;
  //for (i = 1; i<=givenC; i++){
    for (j = 1; j <= givenR; j++){
      long checked = 0;
      double usp_total = 0;
      double nonusp_total = 0;
      long usps = 0;
      long nonusps = 0;
      double total = 0;
      struct timespec begin={0,0}, end={0,0};

      puzzle * p;
      p = create_puzzle_from_index(j,i,0);
<<<<<<< HEAD
      for (index = 0; index < 2500; index++){
=======
      for (index = 0; index < 10; index++){
>>>>>>> af9a8ffcac457f16946ae9d8387f71c977919d77
        //puzzle *p;
        //p = create_puzzle_from_index(j,i,index);
        clock_gettime(CLOCK_MONOTONIC, &begin);
        int result1 = DM_to_MIP(p, env);
        clock_gettime(CLOCK_MONOTONIC, &end);
        //int result2 = check(p -> puzzle, p->row, p -> column);
        // if (result1 != result2){
        //   printf("There are disagreement, 3DM is %d and check is %d", result1, result2);
        //   print_puzzle(p);
        //   return -1;
        // }

        if //(check(p->puzzle, p->row, p->column)){
          (result1){
          //(popen_simple(p->row, p->column,-1,p)){

          usp_total = usp_total + ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
          usps++;
        }else {
          clock_gettime(CLOCK_MONOTONIC, &end);
          nonusp_total = nonusp_total + ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
          nonusps++;
        }
        checked++;
        total = nonusp_total + usp_total;
        randomize_puzzle(p);
        //destroy_puzzle(p);
      }
      destroy_puzzle(p);
      printf("checked: %ld usps: %ld nonusps: %ld\n",checked, usps, nonusps);
      printf("usp average time: %.6f nonusps average time: %.6f \n",
            usp_total/usps, nonusp_total/nonusps);
      printf("total average time: %.6f\n", total/checked);
      printf("finish checking%d by %d\n", j, i);
      }
  GRBfreeenv(env);
  }
