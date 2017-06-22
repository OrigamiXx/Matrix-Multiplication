/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(degree_weight,DegreeWeight)

#else

#ifndef OINK_DEGREE_WEIGHT_H
#define OINK_DEGREE_WEIGHT_H

#include "command.h"

namespace OINK_NS {

class DegreeWeight : public Command {
 public:
  DegreeWeight(class OINK *);
  void run();
  void params(int, char **);

 private:
  int duplicate;

  static void print(char *, int, char *, int, void *);

  static void inverse_degree(char *, int, char *, int, int *, 
			     KeyValue *, void *);
};

}

#endif
#endif
