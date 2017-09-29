/*
  This module checks whether a puzzle is a strong USP by reducing the
  3DM matching instance implicit in the puzzle to a mixed integer
  program and then solve the mixed integer program using Gurobi.  This
  module requires that Gurobi have been installed on the machine which
  this source is compiled on.

  Author: Anthony Yang Xu & Matt.
  Summer 2017.
*/

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#include "gurobi_c++.h"
#include "3DM_to_MIP.h"


//Convert the edge coordinate (r1, r2, r3) in the 3D matching into a
//variable number in {0, ..., s^3-1}.
inline int coord_to_var(int s, int r1, int r2, int r3){
  return r1 * s * s + r2 * s + r3;
}

// Global variable for storing the Gurobi environment.  An
// optimization to prevent Gurobi from querying license server (a
// relatively slow operation) every time we want to use it.
GRBenv *env = NULL;

// Loads the system Gurobi environment to a global variable if it has
// not already been loaded.  Suppresses output of Gurobi's start-up
// message by directing stdout.  XXX - Not sure whether this is thread
// safe.  Returns true iff the Gurobi environment exists on return.
bool ensure_env_loaded_quiet(void){

  if (env == NULL) {
    char buf[20];
    int saved_stdout = dup(1);
    assert(freopen("/dev/null", "w", stdout));
    int res = GRBloadenv(&env, NULL);
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    assert(freopen(buf, "w", stdout));
    if (res != 0) {
      env = NULL;
      return false;
    }
  }

  return true;
}

// Deallocates the global Gurobi environment if allocated.
void finalize_check_MIP(){

  if (env != NULL) {
    GRBfreeenv(env);
    env = NULL;
  }
}

// Builds a mixed integer program in the Gurobi model, which is
// feasible iff p -> tdm has a 3DM that is not the diagonal.  Uses the
// s^3 0-1 variables x_{r1,r2,r3} for all r1, r2, r3 in [s].  Creates
// linear constraints with total size \Omega(s^3).
void construct_reduction(puzzle * p, GRBmodel * model) {

  int s = p -> s;
  puzzle_row num_vars = s * s * s;

  int ind[num_vars];
  double val[num_vars];
  char vtype[num_vars];

  // Add s^3 binary variables {0, ... , s^3-1} to model.
  for (unsigned int i = 0; i < num_vars; i++){
    vtype[i] = GRB_BINARY;
  }
  GRBaddvars(model, num_vars, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);

  // Add constraints for missing 3DM edges.
  // x_{r1,r2,r3} <= !tdm(r1, r2, r3), for all r1, r2, r3.
  for (int r1 = 0; r1 < s; r1++)
    for (int r2 = 0; r2 < s; r2++)
      for (int r3 = 0; r3 < s; r3++)
        if (!get_tdm_entry(p, r1, r2, r3)){
	  val[0] = 1;
          ind[0] = coord_to_var(s, r1, r2, r3);
          GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 0.0, NULL);
        }

  // Add constraints to force one edge matched per layer and slice.
  // SUM_{r2,r3} x_{r1,r2,r3} = 1, for all r1.
  for(int r1 = 0; r1 < s; r1++){
    int counter = 0;
    for(int r2 = 0; r2 < s; r2++){
      for(int r3 = 0; r3 < s; r3++){
	val[counter] = 1;
	ind[counter] = coord_to_var(s, r1, r2, r3);
	counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  // SUM_{r1,r3} x_{r1,r2,r3} = 1, for all r2.
  for(int r2 = 0; r2 < s; r2++){
    int counter = 0;
    for(int r1 = 0; r1 < s; r1++){
      for(int r3 = 0; r3 <s; r3++){
	val[counter] = 1;
	ind[counter] = coord_to_var(s, r1, r2, r3);
	counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  // SUM_{r1,r2} x_{r1,r2,r3} = 1, for all r3.
  for(int r3 = 0; r3 < s; r3++){
    int counter = 0;
    for(int r1 = 0; r1 < s; r1++){
      for(int r2 = 0; r2 <s; r2++){
	val[counter] = 1;
	ind[counter] = coord_to_var(s, r1, r2, r3);
	counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  // Add constraint for exclude diagonal of 3DM instance.
  // SUM_{r1} x_{r1,r1,r1} <= s-1
  for(int r1 = 0; r1 < s; r1++){
    val[r1] = 1;
    ind[r1] = coord_to_var(s, r1, r1, r1);
  }
  GRBaddconstr(model, s, ind, val, GRB_LESS_EQUAL, (double)(s - 1), NULL);


}

// Checks whether p is a strong USP, using the provided Gurobi model.
// Returns 1 if p is a strong USP, 0 if p is not a strong USP and -1
// if it was unable to determine whether p is a strong USP.  -1 will
// only be returned if there was an error, or the solving was
// interrupted.
check_t check_MIP(puzzle *p, GRBmodel * model){

  GRBenv * menv = GRBgetenv(model);

  // Set parameters.
  GRBsetintparam(menv, "Threads", 1);
  GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1);
  GRBsetintparam(menv, "OutputFlag", 0);

  GRBsetdblparam(menv, "FeasibilityTol",1e-9);
  GRBsetdblparam(menv, "IntFeasTol",1e-9);
  GRBsetdblparam(menv, "MarkowitzTol",1e-4);
  GRBsetdblparam(menv, "MIPGap", 0);
  GRBsetdblparam(menv, "MIPGapAbs", 0);
  GRBsetdblparam(menv, "OptimalityTol", 1e-9);
  GRBsetdblparam(menv, "PSDTol", 0);


  // Construct MIP instance.
  construct_reduction(p, model);
  //GRBwrite(model, "3DM_to_MIP.lp");

  GRBoptimize(model);

  /* Capture solution information */
  int opt_status;
  GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);

  if (opt_status == GRB_INFEASIBLE)
    return IS_USP;
  else if (opt_status == GRB_OPTIMAL)
    return NOT_USP;
  else
    return UNDET_USP;

}

// Checks whether p is a strong USP, using a MIP reduction.  Returns 1
// if p is a strong USP, 0 if p is not a strong USP and -1 if it was
// unable to determine whether p is a strong USP.  -1 will only be
// returned if there was an error.
check_t check_MIP(puzzle *p){

  if (!ensure_env_loaded_quiet())
    return UNDET_USP;

  GRBmodel * model = NULL;
  GRBnewmodel(env, &model, "3DM_to_MIP", 0, NULL, NULL, NULL, NULL, NULL);


  check_t res = check_MIP(p, model);

  GRBfreemodel(model);

  return res;

}


// Callback for interrupting Gurobi whenever the bool * in usrdata
// points to true.
int interrupt_callback(GRBmodel *model, void *cbdata, int where, void *usrdata){

  bool * interrupt = ((bool *)usrdata);
  if (*interrupt) {
    GRBterminate(model);
    *interrupt = false;
  }

  return 0;
}

// A synchronized pthread version of check_MIP that takes it's
// parameters using an argument struct.  May be asynchronously
// interrupted by setting args -> interrupt = true.  Returns value
// computed via pthread_exit.  Return value only meaningful if not
// interrupted.
void * MIP(void * arguments){

  thread_args * args = (thread_args *)arguments;

  check_t res = UNDET_USP;

  if (ensure_env_loaded_quiet()){

    // Allocate model.
    GRBmodel *model = NULL;
    GRBnewmodel(env, &model, "3DM_to_MIP", 0, NULL, NULL, NULL, NULL, NULL);
    GRBsetcallbackfunc(model, interrupt_callback, &(args -> interrupt));

    // Run check
    res = check_MIP(args -> p, model);

    // Deallocate model.
    GRBfreemodel(model);

    args -> complete = true;
    sem_post(args -> complete_sem);

  }

  pthread_exit((void*)res);
}
