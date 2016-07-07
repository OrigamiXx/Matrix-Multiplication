/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_OINK_H
#define OINK_OINK_H

#include "mpi.h"
#include "stdio.h"

namespace OINK_NS {

class OINK {
 public:
                                 // ptrs to fundamental OINK classes
  class Memory *memory;          // memory allocation functions
  class Error *error;            // error handling
  class Universe *universe;      // universe of processors
  class Input *input;            // input script processing
                                 // ptrs to top-level OINK-specific classes
  class Object *obj;             // MapReduce objects
  class MRMPI *mrmpi;            // wrapper on MR-MPI library methods

  MPI_Comm world;                // MPI communicator
  FILE *infile;                  // infile
  FILE *screen;                  // screen output
  FILE *logfile;                 // logfile

  OINK(int, char **, MPI_Comm);
  ~OINK();
  void create();
  void init();
  void destroy();
};

}

#endif
