/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_COMMAND_H
#define OINK_COMMAND_H

#include "pointers.h"

namespace OINK_NS {

class Command : protected Pointers {
 public:
  Command(class OINK *);
  virtual ~Command() {}
  virtual void run() = 0;
  virtual void params(int, char **) = 0;

  void inputs(int, char **);
  void outputs(int, char **);

 protected:
  int ninputs,noutputs;
};

}

#endif
