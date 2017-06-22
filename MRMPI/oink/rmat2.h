/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(rmat2,RMAT2)

#else

#ifndef OINK_RMAT2_H
#define OINK_RMAT2_H

#include "command.h"
#include "map_rmat_generate.h"

namespace OINK_NS {

class RMAT2 : public Command {
 public:
  RMAT2(class OINK *);
  void run();
  void params(int, char **);

 private:
  RMAT_struct rmat;
};

}

#endif
#endif
