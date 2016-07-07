/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(histo,Histo)

#else

#ifndef OINK_HISTO_H
#define OINK_HISTO_H

#include "command.h"

namespace OINK_NS {

class Histo : public Command {
 public:
  Histo(class OINK *);
  void run();
  void params(int, char **);

 private:
  static void print(char *, int, char *, int, void *);
};

}

#endif
#endif
