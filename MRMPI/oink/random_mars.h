/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_RANMARS_H
#define OINK_RANMARS_H

#include "pointers.h"

namespace OINK_NS {

class RanMars : protected Pointers {
 public:
  RanMars(class OINK *, int);
  ~RanMars();
  double uniform();
  double gaussian();

 private:
  int seed,save;
  double second;
  double *u;
  int i97,j97;
  double c,cd,cm;
};

}

#endif
