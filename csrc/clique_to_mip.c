#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#include "constants.h"
#include "gurobi_c++.h"
#include "clique_to_mip.h"
#include "puzzle.h"
#include "assert.h"

GRBenv * clique_mip_env = NULL;

bool ensure_clique_env_loaded_quiet(void){

  if (clique_mip_env == NULL) {
    char buf[20];
    int saved_stdout = dup(1);
    assert(freopen("/dev/null", "w", stdout));
    int res = GRBloadenv(&clique_mip_env, NULL);
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    assert(freopen(buf, "w", stdout));
    if (res != 0) {
      clique_mip_env = NULL;
      return false;
    }
  }

  return true;
}


void construct_clique_mip_reduction(ExtensionGraph * eg, GRBmodel * model) {
  unsigned long num_vars = eg -> size() + 1; // +1 for our extra counter

  char * vtype = (char *) malloc(sizeof(char) * num_vars); 

  vtype[num_vars - 1] = GRB_INTEGER;

  for (unsigned long i = 0; i < num_vars - 1; i++) {
    vtype[i] = GRB_BINARY;
  }

  // objective function to maximize the first variable
  double * obj = (double *) malloc(sizeof(double) * num_vars);
  bzero(obj, sizeof(double) * num_vars);

  GRBaddvars(model, num_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);

  int * ind = (int *) malloc (sizeof(int) * 2);
  double * val = (double *) malloc(sizeof(double) * 2);

  for (unsigned long u = 0; u < eg -> size() - 1; u++){
    for (unsigned long v = u+1; v < eg -> size(); v++){
      if (!eg -> hasEdge(u,v)) {
        ind[0] = u;
        ind[1] = v;

        val[0] = 1.0;
        val[1] = 1.0;

        // Xu + Xv <= 1
        // Xu + Xv >= 0
        GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
      }
    }
  }

  int * ind_all = (int *) malloc(sizeof(int) * num_vars);
  double * val_all = (double *) malloc(sizeof(double) * num_vars);
  
  bzero(ind_all, sizeof(int) * num_vars);
  bzero(val_all, sizeof(double) * num_vars);

  ind_all[num_vars - 1] = num_vars - 1;
  val_all[num_vars - 1] = 1;
  for (unsigned long u = 0; u < eg -> size(); u++) {
    ind_all[u] = u;
    val_all[u] = -1;
  }


  int error;
  error = GRBaddconstr(model, num_vars, ind_all, val_all, GRB_EQUAL, 0.0, NULL);
  if (error) {
    printf("error: %s %d\n", GRBgeterrormsg(clique_mip_env), error);
    assert(false);
  }

  free(vtype);
  free(ind);
  free(obj);
  free(val);
  free(ind_all);
  free(val_all);

}

int get_max_clique_mip(ExtensionGraph * eg, GRBmodel * model) {

  int error;

  GRBenv * menv = GRBgetenv(model);

  GRBsetintparam(menv, "Threads", 8);
  GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1);
  GRBsetintparam(menv, "OutputFlag", 0);
  // GRBsetstrparam(menv, "LogFile", "gurobioutput.out");

  // GRBsetintparam(menv, "SolutionLimit", 1);

  int res = 0;
  res = res | GRBsetdblparam(menv, "FeasibilityTol",1e-6);
  res = res | GRBsetdblparam(menv, "IntFeasTol",1e-6);
  res = res | GRBsetdblparam(menv, "MarkowitzTol",1e-4);
  res = res | GRBsetdblparam(menv, "MIPGap", 0);
  res = res | GRBsetdblparam(menv, "MIPGapAbs", 0);
  res = res | GRBsetdblparam(menv, "OptimalityTol", 1e-9);
  res = res | GRBsetdblparam(menv, "PSDTol", 0);
  res = res | GRBsetdblparam(menv, "TimeLimit", 60);

  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
    printf("error: %s\n", GRBgeterrormsg(clique_mip_env));
  }

  construct_clique_mip_reduction(eg, model);

  error = GRBoptimize(model);
  if (error) {
    printf("error: %s\n", GRBgeterrormsg(clique_mip_env));
  }

  int opt_status;
  GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
 
  assert(opt_status == GRB_OPTIMAL || opt_status == GRB_TIME_LIMIT);

  double clique_val;
  double clique_bound;

  switch(opt_status) {
    case GRB_OPTIMAL:
      error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &clique_val);
      //printf("Gurobi found optimal solution => %f\n", clique_val);
      break;

    case GRB_TIME_LIMIT:
      error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &clique_val);
      error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJBOUND, &clique_bound);
      printf("Gurobi timed out, bound is => %f ; search was at %f\n", clique_bound, clique_val);      
      break;
  }

  if (error) {
    printf("error: %s %d\n", GRBgeterrormsg(clique_mip_env), error);
    assert(false);
  }

  return clique_val;

}


int max_clique_mip(ExtensionGraph *eg) {
  assert(ensure_clique_env_loaded_quiet());

  GRBmodel * model = NULL;
  GRBnewmodel(clique_mip_env, &model, "clique_to_mip", 0, NULL, NULL, NULL, NULL, NULL);

  int max_clique = get_max_clique_mip(eg, model);

  GRBfreemodel(model);

  return max_clique;
}
