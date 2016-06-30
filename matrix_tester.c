#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include "permutation.h"
#include "groups.h"
#include "matrix.h"
#include "usp.h"
#include <time.h>


int main(int argc, char * argv[]) {

  puzzle * p = create_puzzle_from_file("puzzles/usp_4_4.puz");

  if (check_usp_mult(p))
    printf("check_usp_mult says p is a USP.\n");
  else
    printf("check_usp_mult says p is NOT a USP.\n");

  if (check_usp(p))
    printf("check_usp says p is a USP.\n");
  else 
    printf("check_usp says p is NOT a USP.\n");

}
