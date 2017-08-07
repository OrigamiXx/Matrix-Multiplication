#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c++.h"
#include "puzzle.h"
#include "usp.h"
#include "usp_bi.h"
#include "checkUSP_mip.h"
#include <unistd.h>
#include <semaphore.h>
#include <sched.h>

int check_MIP(puzzle * p, GRBmodel * model);

//Conver the corrdiate for 3D matching into one index in the format of ijk
int corr_to_index(int s, int i, int j, int k){
  int index;
  index = k + j * s + i * s *s;
  return index;
}



GRBenv *env = NULL;

// Frees statically allocated data.
void finalize_check_MIP(){

  if (env != NULL) {
    GRBfreeenv(env);
    env = NULL;
  }
}


int check_MIP(puzzle *p, GRBmodel * model, bool * interrupt_ptr){



  int s, i, j, k, index, max_index, counter;
  s = p -> row;
  max_index = s * s *s  -1;



  // GRBmodel *model = NULL;
  int       ind[max_index];
  double    val[max_index];
  char      vtype[max_index];
  int       optimstatus;
  //double    objval;
  GRBenv *menv;

  //Set up environment and an empty model
  //GRBloadenv(&env, NULL);

  menv = GRBgetenv(model);
  
  GRBsetintparam(menv, "Threads", 2);
  GRBsetintparam(menv, GRB_INT_PAR_MIPFOCUS, 1);
  //if (interrupt_ptr == NULL)
  GRBsetintparam(menv, "OutputFlag", 0);
  //printf("\n\nThe error is %d\n\n", error);
  //GRBsetdblparam(env, GRB_DBL_PAR_HEURISTICS, 0.0);

  //Set all variable to binary and add all variable to the model.
  for (index = 0; index <= max_index; index++){
    vtype[index] = GRB_BINARY;
  }
  GRBaddvars(model, max_index+1, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);

  //Add constraint to model
  //Add constraint from the model
  for(i = 0; i < s; i++){
    for(j = 0; j < s; j++){
      for(k = 0; k <s; k++){
        index = corr_to_index(s, i, j, k);
        //Initialize all constant to each variable.
        val[index] = 1;
        if (check_usp_rows(i, j, k, p)){

          ind[0] = index;

          GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 0.0, NULL);
        }

      }
    }
  }

  //Add constraint from 3D Matching's uniqueness and existance
  //Add constraint from each i layers
  for(i = 0; i < s; i++){
    counter = 0;
    for(j = 0; j < s; j++){
      for(k = 0; k <s; k++){
      index = corr_to_index(s, i, j, k);
      ind[counter] = index;
      //val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  //Add constraint from each j layers
  for(j = 0; j < s; j++){
    counter = 0;
    for(i = 0; i < s; i++){
      for(k = 0; k <s; k++){
      index = corr_to_index(s, i, j, k);
      ind[counter] = index;
      //val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  //Add constraint from each k layers
  for(k = 0; k <s; k++){
    counter = 0;
    for(j = 0; j < s; j++){
      for(i = 0; i < s; i++){
      index = corr_to_index(s, i, j, k);
      ind[counter] = index;
      //val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 1.0, NULL);
  }

  //Add constraint for checking diagonal line. So there can't be X111, X222, X333...Xiii
  for(counter = 0; counter < s; counter++){
    index = corr_to_index(s, counter, counter, counter);
    ind[counter] = index;
    //val[counter] = 1;
  }
  double limit = (double)(s -1);
  GRBaddconstr(model, counter, ind, val, GRB_LESS_EQUAL, limit, NULL);

  GRBoptimize(model);
  //GRBwrite(model, "3DM_to_MIP.lp");
  
  /* Capture solution information */
  GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);

  //GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);

  /* Free model */

  //GRBfreemodel(model);

  //GRBfreeenv(menv);
  menv = NULL;
  /* Free environment */

  // GRBfreeenv(env);
  //printf("The optimastatus is %d", optimstatus);
  // printf("%d", optimstatus);
  // printf("%d", GRB_OPTIMAL);
  //
  // printf("\nOptimization complete\n");
  if (optimstatus == GRB_INFEASIBLE) {
    return 1;

    //  printf("  x=%.0f, y=%.0f, z=%.0f\n", sol[0], sol[1], sol[2]);
  } else if (optimstatus == GRB_OPTIMAL) {
    return 0;
  } else {
    return -1;
  }
}

int interrupt_callback(GRBmodel *model, void *cbdata, int where, void *usrdata){

  
  bool * interrupt = ((bool *)usrdata);
  if (*interrupt) {
    GRBterminate(model);
    *interrupt = false;
  }
}

// Dummy code for MIP thread.
void * MIP(void * arguments){

  struct thread *args = (struct thread *)arguments;

  if (env == NULL) {
    // Loaded env, but redirect stdout to dev/null to hide init message.
    char buf[20];
    int saved_stdout = dup(1);
    freopen("/dev/null", "w", stdout);
    GRBloadenv(&env, NULL);
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    freopen(buf, "w", stdout);
  }
  long res = -1;
  
  // Allocate model.
  GRBmodel *model = NULL;
  GRBnewmodel(env, &model, "3DM_to_MIP", 0, NULL, NULL, NULL, NULL, NULL);
  GRBsetcallbackfunc(model, interrupt_callback, &(args -> interrupt));
  
  // Run check
  res = check_MIP(args -> p, model, &(args -> interrupt));

  // Deallocate model.
  GRBfreemodel(model);

  args -> complete = true;
  sem_post(args -> complete_sem);

  
  pthread_exit((void*)res);
}


int check_MIP(puzzle *p){
  if (env == NULL) {
    char buf[20];
    int saved_stdout = dup(1);
    freopen("/dev/null", "w", stdout);
    int res = GRBloadenv(&env, NULL);
    sprintf(buf, "/dev/fd/%d", saved_stdout);
    freopen(buf, "w", stdout);
    if (res != 0) {
      env = NULL;
      return -1;
    }
  }
  long res = -1;
  GRBmodel * model = NULL;
  GRBnewmodel(env, &model, "3DM_to_MIP", 0, NULL, NULL, NULL, NULL, NULL);
  res = check_MIP(p, model, NULL);
  GRBfreemodel(model);
  return res;
}
