/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

// Pointers class contains ptrs to master copy of
//   fundamental OINK class ptrs stored in mrmpi.h
// every OINK class inherits from Pointers to access mrmpi.h ptrs
// these variables are auto-initialized by Pointer class constructor
// *& variables are really pointers to the pointers in mrmpi.h
// & enables them to be accessed directly in any class, e.g. error->all()

#ifndef OINK_POINTERS_H
#define OINK_POINTERS_H

#include "mpi.h"
#include "oink.h"

namespace OINK_NS {

class Pointers {
 public:
  Pointers(OINK *ptr) : 
    oink(ptr),
    memory(ptr->memory),
    error(ptr->error),
    universe(ptr->universe),
    input(ptr->input),
    obj(ptr->obj),
    mrmpi(ptr->mrmpi),
    world(ptr->world),
    infile(ptr->infile),
    screen(ptr->screen),
    logfile(ptr->logfile) {}
  virtual ~Pointers() {}

 protected:
  OINK *oink;
  Memory *&memory;
  Error *&error;
  Universe *&universe;
  Input *&input;

  Object *&obj;
  MRMPI *&mrmpi;

  MPI_Comm &world;
  FILE *&infile;
  FILE *&screen;
  FILE *&logfile;
};

}

#endif
