#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "puzzle.h"
#include "usp.h"

int power(int base, int exponent){
  int result = 1;
  for (int i = 0; i < exponent; i++){
    result = result * base;
  }
  return result;
}

//Conver the corrdiate for 3D matching into one index in the format of ijk
int corr_to_index(int s, int i, int j, int k){
  int index;
  index = k + j * power(s, 1) + i * power(s, 2);
  return index;
}

int DM_to_MIP(puzzle *p){


  int s, i, j, k, index, max_index, counter;
  s = p -> row;
  max_index = power(s, 3) -1;

  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       ind[max_index];
  double    val[max_index];
  char      vtype[max_index];
  int       optimstatus;
  double    objval;


  //Set up environment and an empty model
  GRBloadenv(&env, "3DM_to_MIP.log");
  GRBnewmodel(env, &model, "3DM_to_MIP", 0, NULL, NULL, NULL, NULL, NULL);

  //Set all variable to binary and add all variable to the model.
  for (index = 0; index <= max_index; index++){
    vtype[index] = GRB_BINARY;
  }
  GRBaddvars(model, max_index, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);

  //Add constraint to model
  //Add constraint from the model
  for(i = 0; i < s; i++){
    for(j = 0; j < s; j++){
      for(k = 0; k <s; k++){
        if (check_usp_rows(i, j, k, p)){
          index = corr_to_index(s, i, j, k);
          ind[0] = index;
          val[0] = 1;

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
      val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 0.0, NULL);
  }

  //Add constraint from each j layers
  for(j = 0; j < s; j++){
    counter = 0;
    for(i = 0; i < s; i++){
      for(k = 0; k <s; k++){
      index = corr_to_index(s, i, j, k);
      ind[counter] = index;
      val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 0.0, NULL);
  }

  //Add constraint from each k layers
  for(k = 0; k <s; k++){
    counter = 0;
    for(j = 0; j < s; j++){
      for(i = 0; i < s; i++){
      index = corr_to_index(s, i, j, k);
      ind[counter] = index;
      val[counter] = 1;
      counter++;
      }
    }
    GRBaddconstr(model, counter, ind, val, GRB_EQUAL, 0.0, NULL);
  }

  //Add constraint for checking diagonal line. So there can't be X111, X222, X333...Xiii
  for(counter = 0; counter < s; counter++){
    index = corr_to_index(s, counter, counter, counter);
    ind[counter] = index;
    val[counter] = 1;
  }
  double limit = (double)(s -1);
  GRBaddconstr(model, counter, ind, val, GRB_LESS_EQUAL, limit, NULL);

  GRBoptimize(model);
  GRBwrite(model, "3DM_to_MIP.lp");

  /* Capture solution information */
  GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);

  GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);

  /* Free model */

  GRBfreemodel(model);

  /* Free environment */

  GRBfreeenv(env);

  printf("\nOptimization complete\n");
  if (optimstatus == GRB_OPTIMAL) {
    printf("Not USP");

  //  printf("  x=%.0f, y=%.0f, z=%.0f\n", sol[0], sol[1], sol[2]);
  } else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Is USP\n");
  } else {
    printf("Optimization was stopped early\n");
  }

  return 0;
}
