#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){


  FILE * cmd_pipe = popen("minisat_solver","w");

  if (cmd_pipe == NULL) {
    printf("Error, unable to executate minisat_solver\n");
  }
  fprintf(cmd_pipe,"p cnf 2 4\n");
  fprintf(cmd_pipe,"2 2 0\n");
  fprintf(cmd_pipe,"1 0\n");
  fprintf(cmd_pipe,"1 0\n");
  //fprintf(cmd_pipe,"-2 -2 0\n");
  //fprintf(cmd_pipe,"1 -2 0\n2 -2 0");
  //fprintf(cmd_pipe,"2 -2 0 ");

  printf("Executing!\n");

  pclose(cmd_pipe);
  
  return 0;
}
