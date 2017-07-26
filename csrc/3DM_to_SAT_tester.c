/* Reduction from 3D-matching to 3SAT
   The goal is to use the result from check_usp_rows to deside whether
   witness is allowed to occur on that specific coordinate on a 3D cube.
   Then use the result to reduce this 3D-perfect matching problem to a 3SAT
   problem and print out the reduction in 3cnf-form (dimacs) for MapleSAT solver
   to give a final answer.

   Author: Jerry
*/
#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"
#include "usp.h"
#include "assert.h"
#include "puzzle.h"
#include <math.h>
#include "usp_bi.h"
#include "3DM_to_SAT.h"
#include <time.h>
#include <sys/time.h>

int main(int argc, char * argv[]){
  int givenR = 15;
  int givenC = 6;
  // int * puzzle1 = (int *) malloc(sizeof(int *)*givenR);
  // puzzle1[0] = 279;
  // puzzle1[1]= 284;
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
  // result->pi = create_perm_identity(result->row);
  // result -> puzzle = puzzle1;
  // print_puzzle(result);
  // file_simple(givenR,givenC, get_index_from_puzzle(result),result);

  int i, j;
  //int checked = 0;
  long index;
  /*for (index=0; index<power(3,givenC*givenR)-1; index+=10){
    puzzle * p;
    p = create_puzzle_from_index(givenR,givenC, index);
    if (popen_simple(givenR,givenC,index,p)!= solver_simple(givenR, givenC, index, p)){
      print_puzzle(p);
      printf("existing a wrong case\n");
      break;}
    checked++;
  }
  printf("checked: %d\n", checked);*/
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
  /*clock_t begin, stop;
  FILE * a;
  begin = clock();
  clock_gettime(CLOCK_MONOTONIC, &start);
  a = popen("sleep 2", "r");
  pclose(a);
  stop = clock();
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("%.5f\n", ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec));
  printf("%.5f\n", ((double) (stop-begin))/ CLOCKS_PER_SEC);*/


  //for (index = 0; index < power(3, i*j) -1; index+=100000000000000){
      puzzle * p;
      p = create_puzzle_from_index(j,i,244);
      for (index = 0; index < 10000; index++){
        //puzzle *p;
        //p = create_puzzle_from_index(j,i,index);
        clock_gettime(CLOCK_MONOTONIC, &begin);
        if //(check(p->puzzle, p->row, p->column)){
          (solver_simple(p->row, p->column,-1,p)){
          //(popen_simple(p->row, p->column,-1,p)){
          clock_gettime(CLOCK_MONOTONIC, &end);
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
  //}

  // if(popen_simple(givenR, givenC, get_index_from_puzzle(result),result)){
  //     printf("UNSAT\n");
  //   }else{
  //     printf("SAT\n");
  // }



  //reduction_to_3cnf(givenR, givenC, get_index_from_puzzle(result), result);
  /*if (check(result->puzzle, result->row, result->column)){
    printf("Yes\n");
  } else{
    printf("%s\n","No" );
  }*/


  //destroy_puzzle(result);
  return 0;
}
