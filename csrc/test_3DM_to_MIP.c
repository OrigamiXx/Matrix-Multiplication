/* 
   ???
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <iostream>
#include <math.h>

#include "permutation.h"
#include "checker.h"
#include "puzzle.h"
#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"


int main(int argc, char * argv[]){
  int givenR = 30;
  int givenC = 9;
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

  int thread_res, mip_res, sat_res;
  double thread_time, mip_time, sat_time;

  FILE * nonUSP = fopen("data/6-row-nonUSP-timing.csv", "w");
  FILE * USP = fopen("data/6-row-USP-timing.csv", "w");
  assert(nonUSP != NULL);
  assert(USP != NULL);


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
  for (j = givenR; j <= givenR; j++){
      int checked = 0;
      double usp_total = 0;
      double nonusp_total = 0;
      long usps = 0;
      long nonusps = 0;
      double total = 0;
      struct timespec begin={0,0}, end={0,0};

      fprintf(nonUSP, "%d * %d\n", j, givenC);
      fprintf(nonUSP, "puzzle_number, thread_check, mip_check, sat_check\n");
      fflush(nonUSP);

      fprintf(USP, "%d * %d\n", j, givenC);
      fprintf(USP, "puzzle_number, thread_check, mip_check, sat_check\n");
      fflush(USP);
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
      //puzzle * p;
      puzzle * p = create_puzzle(j, i);
      randomize_puzzle(p);
      srand48(time(NULL));
      for (index = 0; index < 100; index++){
	printf("index = %ld\n", index);
        //puzzle *p;
        //p = create_puzzle_from_index(j,i,index);

        clock_gettime(CLOCK_MONOTONIC, &begin);
        mip_res = check_MIP(p);
        clock_gettime(CLOCK_MONOTONIC, &end);
        mip_time = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);


        clock_gettime(CLOCK_MONOTONIC, &begin);
        sat_res = check_SAT(p);
        clock_gettime(CLOCK_MONOTONIC, &end);
        sat_time = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);


        clock_gettime(CLOCK_MONOTONIC, &begin);
        thread_res = check_SAT_MIP(p);
        clock_gettime(CLOCK_MONOTONIC, &end);
        thread_time = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);

	
        if(mip_res != sat_res || sat_res != thread_res){
          printf("Conflict!");
          printf("mip is %d, sat is %d, and thread is %d", mip_res, sat_res, thread_res);
          break;
        }
        else if (thread_res == 1){
          usps++;
          fprintf(USP, "%d,%.5f,%.5f,%.5f\n", checked, thread_time, mip_time, sat_time);

        }

        else if (thread_res == 0){
          nonusps++;
          fprintf(nonUSP, "%d,%.5f,%.5f,%.5f\n", checked, thread_time, mip_time, sat_time);
        }

        else{
          printf("Unexpected problem");
          return -1;
        }
        // clock_gettime(CLOCK_MONOTONIC, &begin);
        // if (check(p->puzzle, p->row, p->column)){
        //   //(check_MIP(p)){
        //   //(check_SAT(p)){
        //   //(popen_simple(p->row, p->column,-1,p)){
        //   clock_gettime(CLOCK_MONOTONIC, &end);
        //   usp_total = usp_total + ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
        //   usps++;
        // }else {
        //   clock_gettime(CLOCK_MONOTONIC, &end);
        //   nonusp_total = nonusp_total + ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
        //   nonusps++;
        // }
        checked++;
        //total = nonusp_total + usp_total;
        randomize_puzzle(p);
        srand48(time(NULL));
        //destroy_puzzle(p);
      }
      destroy_puzzle(p);
      printf("checked: %d usps: %ld nonusps: %ld\n",checked, usps, nonusps);
      //printf("usp average time: %.6f nonusps average time: %.6f \n",
      //      usp_total/usps, nonusp_total/nonusps);
      //printf("total average time: %.6f\n", total/checked);
      printf("finish checking%d by %d\n", j, i);
    }
  finalize_check_MIP();
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
