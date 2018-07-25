#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#include "constants.h"
#include "gurobi_c++.h"
#include "clique_to_mip.h"
#include "puzzle.h"
#include "assert.h"
#include "checker.h"

#define SAFE_GUROBI(e) if (e) { fprintf(stderr,"%s %d\n", GRBgeterrormsg(search_mip_env), e); assert(false);}

GRBenv * search_mip_env = NULL;

bool ensure_search_env_loaded_quiet(void){

  if (search_mip_env == NULL) {
    char buf[20];
    int saved_stdout = dup(1);
    assert(freopen("/dev/null", "w", stdout));
    int res = GRBloadenv(&search_mip_env, NULL);
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    assert(freopen(buf, "w", stdout));
    if (res != 0) {
      search_mip_env = NULL;
      return false;
    }
  }

  return true;
}


void construct_initial_constraints(int s, int k, GRBmodel * model) {
  puzzle_row max_index = MAX_ROWS[k];
  puzzle_row num_vars = max_index + 1;
  
  char * vtype = (char *) malloc(sizeof(char) * num_vars); 

  for (puzzle_row i = 0; i < num_vars - 1; i++) {
    vtype[i] = GRB_BINARY;
  }
  
  vtype[num_vars - 1] = GRB_CONTINUOUS; //GRB_INTEGER;

  // objective function to maximize the last variable
  double * obj = (double *) malloc(sizeof(double) * num_vars);
  obj[num_vars - 1] = 1;
  for (puzzle_row i = 0; i < num_vars - 1; i++) {
    obj[i] = 0;
  }

  SAFE_GUROBI(GRBaddvars(model, num_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL));

  int * ind = (int *) malloc (sizeof(int) * 2);
  double * val = (double *) malloc(sizeof(double) * 2);

  puzzle * p = create_puzzle(2,k);
  for (puzzle_row u = 0; u < max_index; u++){
    p -> puzzle[0] = u;
    for (puzzle_row v = u+1; v < max_index; v++){
      p -> puzzle[1] = v;
      if (check(p) == NOT_USP){
        ind[0] = u;
        ind[1] = v;

        val[0] = 1.0;
        val[1] = 1.0;
	
        SAFE_GUROBI(GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 1.0, NULL));
      }
    }
  }

  /*
  destroy_puzzle(p);
  p = create_puzzle(3,k);
  
  for (puzzle_row u = 0; u < max_index; u++){
    p -> puzzle[0] = u;
    printf("\r%7.3f%%", u / (float) max_index * 100.0);
    fflush(stdout);
    for (puzzle_row v = u+1; v < max_index; v++){
      p -> puzzle[1] = v;
      for (puzzle_row w = v+1; w < max_index; w++){
	p -> puzzle[2] = w;
	if (check(p) == NOT_USP){
	  ind[0] = u;
	  ind[1] = v;
	  ind[2] = w;
	  
	  val[0] = 1.0;
	  val[1] = 1.0;
	  val[2] = 1.0;
	    
	  SAFE_GUROBI(GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 2.0, NULL));
	}
      }
    }
  }
  */

  if (s != 0) {
    //double relax_s = 15.01796;
    double relax_s = s;
    
    ind[0] = num_vars - 1;
    val[0] = 1.0;
    SAFE_GUROBI(GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, relax_s, NULL));
  }
    
  
  int * ind_all = (int *) malloc(sizeof(int) * num_vars);
  double * val_all = (double *) malloc(sizeof(double) * num_vars);
  
  bzero(ind_all, sizeof(int) * num_vars);
  bzero(val_all, sizeof(double) * num_vars);

  ind_all[num_vars - 1] = num_vars - 1;
  val_all[num_vars - 1] = 1;
  for (puzzle_row u = 0; u < max_index; u++) {
    ind_all[u] = u;
    val_all[u] = -1.0 - (double)(max_index - u - 1) / (double)(max_index * max_index);
  }

  int error;
  SAFE_GUROBI(GRBaddconstr(model, num_vars, ind_all, val_all, GRB_EQUAL, 0.0, NULL));

  puzzle * feasible_soln = NULL;
  // Set known feasible soln.
  switch (k) {
  case 1:
    feasible_soln = create_puzzle_from_string((char *)"1\n");
    break;
  case 2:
    feasible_soln = create_puzzle_from_string((char *)"13\n21\n");
    break;
  case 3:
    feasible_soln = create_puzzle_from_string((char *)"111\n321\n332\n");
    break;
  case 4:
    feasible_soln = create_puzzle_from_string((char *)"3132\n1232\n1113\n3213\n3323\n");
    break;
  case 5:
    feasible_soln = create_puzzle_from_string((char *)"33311\n11221\n21332\n32223\n21213\n22312\n32321\n31211\n");
    break;
  case 6:
    feasible_soln = create_puzzle_from_string((char *)"233111\n211211\n331211\n322211\n231121\n223121\n331321\n323321\n211312\n231322\n311113\n332313\n332123\n223223\n");
    break;    
  default:
    break;
  }

  if (feasible_soln != NULL){
    printf("Starting with known feasible solution.\n");
    assert(IS_USP == check(feasible_soln));
    for (int i = 0; i < num_vars; i++){
      GRBsetdblattrelement(model, "Start", i, 0.0);
    }
    for (int i = 0; i < feasible_soln -> s; i++){
      GRBsetdblattrelement(model, "Start", feasible_soln -> puzzle[i], 1.0);
    }
    GRBsetdblattrelement(model, "Start", num_vars-1, GRB_UNDEFINED);//feasible_soln -> s);
  }

  destroy_puzzle(feasible_soln);
  destroy_puzzle(p);
  free(vtype);
  free(ind);
  free(obj);
  free(val);
  free(ind_all);
  free(val_all);

}

int __stdcall check_callback(GRBmodel * model, void *cbdata, int where, void *usrdata){
 
  if (where != GRB_CB_MIPSOL)
    return 0;

  //printf("In callback!\n");
  
  int k = (int)(long)usrdata;
  puzzle_row max_index = MAX_ROWS[k];
  puzzle_row num_vars = max_index + 1;
  static double* sol = (double *)malloc(sizeof(double) * num_vars);
  
  GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, sol);

  static int * constraint_indexes = (int *) malloc(sizeof(int) * num_vars);
  static double * constraint_coefs = (double *) malloc(sizeof(double) * num_vars);
  
  //bzero(constraint_indexes, sizeof(int) * num_vars);
  //bzero(constraint_coefs, sizeof(double) * num_vars);
  
  static puzzle * p = create_puzzle(max_index, k);
  p -> s = 0;

  // Loop over rows in solution added to puzzle until not a SUSP.
  // Resets and keeps trying to add constraints.
  for (int i = 0; i < max_index; i++){
    if (sol[i] < 0.5) continue;
    constraint_indexes[p -> s] = i;
    constraint_coefs[p -> s] = 1;
    p -> puzzle[p -> s++] = i;
    if (NOT_USP == check(p)){
      SAFE_GUROBI(GRBcblazy(cbdata, p -> s, constraint_indexes, constraint_coefs, GRB_LESS_EQUAL, p -> s - 1));
      p -> s = 0;
      break;
    }
  }

  //destroy_puzzle(p);
  //free(constraint_indexes);
  //free(constraint_coefs);
  //free(sol);

  //printf("Leaving Callback\n");
  return 0;
}

int search_mip(int s, int k, GRBmodel * model) {

  int error;

  GRBenv * menv = GRBgetenv(model);

  SAFE_GUROBI(GRBsetintparam(menv, "Threads", 4));
  SAFE_GUROBI(GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1));
  SAFE_GUROBI(GRBsetintparam(menv, "OutputFlag", 1));
  // SAFE_GUROBI(GRBsetintparam(menv, "SolutionLimit", 1));

  SAFE_GUROBI(GRBsetdblparam(menv, "FeasibilityTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "IntFeasTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "MarkowitzTol",1e-4));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGap", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGapAbs", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "OptimalityTol", 1e-9));
  SAFE_GUROBI(GRBsetdblparam(menv, "PSDTol", 0));

  SAFE_GUROBI(GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE));

  construct_initial_constraints(s, k, model);

  SAFE_GUROBI(GRBsetcallbackfunc(model, check_callback, (void *)(long)k));
  SAFE_GUROBI(GRBsetintparam(menv, GRB_INT_PAR_LAZYCONSTRAINTS, 1));
  SAFE_GUROBI(GRBsetintparam(menv, "CliqueCuts", 2));
  
  SAFE_GUROBI(GRBoptimize(model));

  int opt_status;
  SAFE_GUROBI(GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status));

 
  assert(opt_status == GRB_OPTIMAL);

  double opt_val;
  
  SAFE_GUROBI(GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &opt_val));

  double sol[MAX_ROWS[k]+1];
  SAFE_GUROBI(GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, MAX_ROWS[k]+1, sol));

  int count = 0;
  puzzle * p = create_puzzle(opt_val, k);
  for (int i = 0; i < MAX_ROWS[k]; i++){
    if (sol[i] < 0.5) continue;
    p -> puzzle[count++] = i;
  }
  assert(count == floor(opt_val));

  print_puzzle(p);
  assert(check(p) == IS_USP);
  
  return count; 

}

int main(int argc, char * argv[]){

  if (argc != 2 && argc != 3){
    fprintf(stderr, "usage: search_MIP <s> <k>\n");
    return -1;
  }
  int s = atoi(argv[1]);
  int k = atoi(argv[2]);

  
  assert(ensure_search_env_loaded_quiet());

  GRBmodel * model = NULL;
  SAFE_GUROBI(GRBnewmodel(search_mip_env, &model, "search_mip", 0, NULL, NULL, NULL, NULL, NULL));

  int max_clique = search_mip(s, k, model);

  SAFE_GUROBI(GRBfreemodel(model));

  return 0;
}
