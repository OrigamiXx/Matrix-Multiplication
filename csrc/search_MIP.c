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
#include "ExtensionGraph.hpp"

#define SAFE_GUROBI(e) if (e) { fprintf(stderr,"%s %d\n", GRBgeterrormsg(search_mip_env), e); assert(false);}

GRBenv * search_mip_env = NULL;

bool ensure_search_mip_env_loaded_quiet(void){

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


void construct_initial_constraints(ExtensionGraph * eg, int s, int k, GRBmodel * model) {

  unsigned long num_vars = eg -> size() + 1;
  
  char vtype[num_vars];
  for (puzzle_row i = 0; i < num_vars - 1; i++) {
    vtype[i] = GRB_BINARY;
  }
  //vtype[num_vars - 1] = GRB_CONTINUOUS;
  vtype[num_vars - 1] = GRB_INTEGER;

  // objective function to maximize the last variable
  double obj[num_vars];
  for (puzzle_row i = 0; i < num_vars - 1; i++) {
    obj[i] = 0;
  }
  obj[num_vars - 1] = 1;
  
  SAFE_GUROBI(GRBaddvars(model, num_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL));

  int ind[2];
  double val[2];

  for (unsigned long u = 0; u < num_vars - 1; u++){
    for (unsigned long v = u+1; v < num_vars - 1; v++){
      if (!eg -> hasEdge(u,v)){
        ind[0] = u;
        ind[1] = v;

        val[0] = 1.0;
        val[1] = 1.0;
	
        SAFE_GUROBI(GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 1.0, NULL));
      }
    }
  }

  if (s != 0) {
    double relax_s = s;
    ind[0] = num_vars - 1;
    val[0] = 1.0;
    SAFE_GUROBI(GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, relax_s, NULL));
  }
    
  
  int ind_all[num_vars];
  double val_all[num_vars];
  
  bzero(ind_all, sizeof(int) * num_vars);
  bzero(val_all, sizeof(double) * num_vars);

  ind_all[num_vars - 1] = num_vars - 1;
  val_all[num_vars - 1] = 1;
  for (puzzle_row u = 0; u < num_vars - 1; u++) {
    ind_all[u] = u;
    val_all[u] = -1.0; // - (double)(num_vars - 1 - u - 1) / (double)((num_vars - 1) * (num_vars - 1));
  }

  SAFE_GUROBI(GRBaddconstr(model, num_vars, ind_all, val_all, GRB_EQUAL, 0.0, NULL));

  /*
  if (s == 0){
    // Set known feasible soln only if initial puzzle is empty.
    puzzle * feasible_soln = NULL;
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
      destroy_puzzle(feasible_soln);
    }
  }
  */
}

struct callback_args{
  ExtensionGraph * eg;
  puzzle * init_p;
};

int __stdcall __check_callback(GRBmodel * model, void *cbdata, int where, void *usrdata){
 
  if (where != GRB_CB_MIPSOL)
    return 0;

  //printf("In callback!\n");

  struct callback_args * args = (struct callback_args *)usrdata;
  puzzle * init_p = args -> init_p;
  ExtensionGraph * eg = args -> eg;
  int k = init_p -> k;
  unsigned long num_vars = eg -> size() + 1;

  double sol[num_vars];
  
  GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, sol);

  int constraint_indexes[num_vars];
  double constraint_coefs[num_vars];
  
  bzero(constraint_indexes, sizeof(int) * num_vars);
  bzero(constraint_coefs, sizeof(double) * num_vars);
  
  static puzzle * p = create_puzzle(init_p -> s + num_vars - 1, k);
  memcpy(p -> puzzle, init_p -> puzzle, sizeof(puzzle_row) * init_p -> s);
  p -> s = init_p -> s;

  // Loop over rows in solution added to puzzle until not a SUSP.
  // Resets and keeps trying to add constraints.
  //printf("num_vars = %ld\n",  num_vars);
  int count = 0;
  for (unsigned long u = 0; u < num_vars - 1; u++){
    if (sol[u] < 0.5) continue;
    constraint_indexes[count] = u;
    constraint_coefs[count] = 1;
    p -> puzzle[p -> s] = eg -> getLabel(u);
    p -> s++;
    count++;
    if (NOT_USP == check(p)){
      SAFE_GUROBI(GRBcblazy(cbdata, count,
			    constraint_indexes, constraint_coefs,
			    GRB_LESS_EQUAL, count - 1));
      p -> s = init_p -> s;
      count = 0;
      //break;
    }
  }

  //printf("Leaving Callback\n");
  return 0;
}



int mip_h(puzzle * init_p, ExtensionGraph * eg, int s) {

  assert(IS_USP == check(init_p));
  
  assert(ensure_search_mip_env_loaded_quiet());
  
  GRBmodel * model = NULL;
  SAFE_GUROBI(GRBnewmodel(search_mip_env, &model, "search_mip", 0, NULL, NULL, NULL, NULL, NULL));
  
  unsigned long num_vars = eg -> size() + 1;
  //  eg.print();
  //printf("num_vars = %ld\n",  num_vars);
  
  GRBenv * menv = GRBgetenv(model);

  SAFE_GUROBI(GRBsetintparam(menv, "Threads", 4));
  SAFE_GUROBI(GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1));
  SAFE_GUROBI(GRBsetintparam(menv, "OutputFlag", 0));
  // SAFE_GUROBI(GRBsetintparam(menv, "SolutionLimit", 1));

  SAFE_GUROBI(GRBsetdblparam(menv, "FeasibilityTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "IntFeasTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "MarkowitzTol",1e-4));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGap", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGapAbs", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "OptimalityTol", 1e-9));
  SAFE_GUROBI(GRBsetdblparam(menv, "PSDTol", 0));

  SAFE_GUROBI(GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE));

  construct_initial_constraints(eg, s - init_p -> s, init_p -> k, model);

  struct callback_args args;
  args.init_p = init_p;
  args.eg = eg;
  
  SAFE_GUROBI(GRBsetcallbackfunc(model, __check_callback, (void *)&args));
  SAFE_GUROBI(GRBsetintparam(menv, GRB_INT_PAR_LAZYCONSTRAINTS, 1));
  SAFE_GUROBI(GRBsetintparam(menv, "CliqueCuts", 2));
  
  SAFE_GUROBI(GRBoptimize(model));

  int opt_status;
  SAFE_GUROBI(GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status));

 
  assert(opt_status == GRB_OPTIMAL);

  double opt_val;
  
  SAFE_GUROBI(GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &opt_val));


  double sol[num_vars];
  SAFE_GUROBI(GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, num_vars, sol));

  int count = 0;
  puzzle * p = extend_puzzle(init_p, opt_val);
  //printf("opt_val = %f\n", opt_val);
  p -> s = init_p -> s;
  for (unsigned long u = 0; u < num_vars - 1; u++){
    if (sol[u] < 0.5) continue;
    p -> puzzle[p -> s] = eg -> getLabel(u);
    p -> s++;
    count++;
  }
  assert(count == floor(opt_val));

  //print_puzzle(p);
  assert(check(p) == IS_USP);

  SAFE_GUROBI(GRBfreemodel(model));
  
  return count; 

}

int mip_h(puzzle * p, ExtensionGraph * eg){

  return mip_h(p, eg, p -> max_row);
  
}


