
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "puzzle.h"
#include "usp.h"

check_t time_check(check_t (* checker)(puzzle *), puzzle * p, double * time_ptr){

  struct timespec begin={0,0}, end={0,0};  

  clockid_t clock_mode = CLOCK_MONOTONIC; //CLOCK_REALTIME; //CLOCK_PROCESS_CPUTIME_ID; 
  
  clock_gettime(clock_mode, &begin);
  check_t res = (*checker)(p);
  clock_gettime(clock_mode, &end);
  *time_ptr = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);
  
  return res;
}
