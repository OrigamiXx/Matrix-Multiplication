/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(edge_upper,EdgeUpper)

#else

#ifndef OINK_EDGE_UPPER_H
#define OINK_EDGE_UPPER_H

#include "command.h"

namespace OINK_NS {

class EdgeUpper : public Command {
 public:
  EdgeUpper(class OINK *);
  void run();
  void params(int, char **);
};

}

#endif
#endif
