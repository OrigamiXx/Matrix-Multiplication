/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_ERROR_H
#define OINK_ERROR_H

#include "pointers.h"

namespace OINK_NS {

class Error : protected Pointers {
 public:
  Error(class OINK *);

  void universe_all(const char *);
  void universe_one(const char *);

  void all(const char *);
  void one(const char *);
  void warning(const char *, int = 1);

  void message(char *, int = 1);
};

}

#endif
