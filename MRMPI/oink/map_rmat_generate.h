/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_RMAT_GENERATE_H
#define OINK_RMAT_GENERATE_H

#include "typedefs.h"

// data structure for RMAT parameters

struct RMAT_struct {
  uint64_t order;
  uint64_t ngenerate;
  int nlevels;
  int nnonzero;
  double a,b,c,d,fraction;
};

#endif
