#ifdef __GUROBI_INSTALLED__

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

#define SAFE_GUROBI(e) {int __result = (e); if (__result) { fprintf(stderr,"%s %d\n", GRBgeterrormsg(clique_mip_env), __result); assert(false);}}


bool ensure_clique_env_loaded_quiet(void){

  if (clique_mip_env == NULL) {
    char buf[20];
    int saved_stdout = dup(1);
    assert(freopen("/dev/null", "w", stdout));
    SAFE_GUROBI(GRBloadenv(&clique_mip_env, NULL));
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    assert(freopen(buf, "w", stdout));
  }

  return true;
}


void construct_clique_mip_reduction(ExtensionGraph * eg, GRBmodel * model) {
  unsigned long num_vars = eg -> size() + 1; // +1 for our extra counter

  char * vtype = (char *) malloc(sizeof(char) * num_vars); 

  vtype[num_vars - 1] = GRB_INTEGER;

  for (unsigned long u = 0; u < num_vars - 1; u++) {
    vtype[u] = GRB_BINARY;  // X_u in {0,1}
  }

  // objective function to maximize the first variable
  double * obj = (double *) malloc(sizeof(double) * num_vars);
  bzero(obj, sizeof(double) * num_vars);
  obj[num_vars - 1] = 1.0;

  SAFE_GUROBI(GRBaddvars(model, num_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL));

  int ind[2];
  double val[2];
  
  for (unsigned long u = 0; u < eg -> size(); u++){
    for (unsigned long v = u+1; v < eg -> size(); v++){
      if (!eg -> hasEdge(u,v)) {
        ind[0] = u;
        ind[1] = v;

        val[0] = 1.0;
        val[1] = 1.0;

        // X_u + X_v <= 1
        SAFE_GUROBI(GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 1.0, NULL));
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

  SAFE_GUROBI(GRBaddconstr(model, num_vars, ind_all, val_all, GRB_EQUAL, 0.0, NULL));

  free(vtype);
  free(obj);
  free(ind_all);
  free(val_all);

}

int get_max_clique_mip(ExtensionGraph * eg, GRBmodel * model) {

  int error;

  GRBenv * menv = GRBgetenv(model);

  SAFE_GUROBI(GRBsetintparam(menv, "Threads", 8));
  SAFE_GUROBI(GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1));
  SAFE_GUROBI(GRBsetintparam(menv, "OutputFlag", 0));
  // SAFE_GUROBI(GRBsetstrparam(menv, "LogFile", "gurobioutput.out"));

  // SAFE_GUROBI(GRBsetintparam(menv, "SolutionLimit", 1));

  SAFE_GUROBI(GRBsetdblparam(menv, "FeasibilityTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "IntFeasTol",1e-6));
  SAFE_GUROBI(GRBsetdblparam(menv, "MarkowitzTol",1e-4));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGap", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "MIPGapAbs", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "OptimalityTol", 1e-9));
  SAFE_GUROBI(GRBsetdblparam(menv, "PSDTol", 0));
  SAFE_GUROBI(GRBsetdblparam(menv, "TimeLimit", 60));

  SAFE_GUROBI(GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE));

  construct_clique_mip_reduction(eg, model);

  SAFE_GUROBI(GRBoptimize(model));

  int opt_status;
  GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
 
  assert(opt_status == GRB_OPTIMAL || opt_status == GRB_TIME_LIMIT);

  double clique_val;
  double clique_bound;

  switch(opt_status) {
    case GRB_OPTIMAL:
      SAFE_GUROBI(GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &clique_val));
      //printf("Gurobi found optimal solution => %f\n", clique_val);
      break;

    case GRB_TIME_LIMIT:
      SAFE_GUROBI(GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &clique_val));
      SAFE_GUROBI(GRBgetdblattr(model, GRB_DBL_ATTR_OBJBOUND, &clique_bound));
      printf("Gurobi timed out, bound is => %f ; search was at %f\n", clique_bound, clique_val);      
      break;
  }

  return (int)round(clique_val);

}


int clique_mip_h(puzzle * p){

  ExtensionGraph g(p);
  return clique_mip_h(&g);

}

int clique_mip_h(ExtensionGraph *eg) {
  assert(ensure_clique_env_loaded_quiet());

  GRBmodel * model = NULL;
  SAFE_GUROBI(GRBnewmodel(clique_mip_env, &model, "clique_to_mip", 0, NULL, NULL, NULL, NULL, NULL));

  int max_clique = get_max_clique_mip(eg, model);

  SAFE_GUROBI(GRBfreemodel(model));

  return max_clique;
}

#endif
