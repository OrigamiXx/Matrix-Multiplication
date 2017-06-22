/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_UNIVERSE_H
#define OINK_UNIVERSE_H

#include "mpi.h"
#include "stdio.h"
#include "pointers.h"

namespace OINK_NS {

class Universe : protected Pointers {
 public:
  char *version;          // OINK version string = date

  MPI_Comm uworld;        // communicator for entire universe
  int me,nprocs;          // my place in universe

  FILE *uscreen;          // universe screen output
  FILE *ulogfile;         // universe logfile

  int existflag;          // 1 if universe exists due to -partition flag
  int nworlds;            // # of worlds in universe
  int iworld;             // which world I am in
  int *procs_per_world;   // # of procs in each world
  int *root_proc;         // root proc in each world

  Universe(class OINK *, MPI_Comm);
  ~Universe();
  void add_world(char *);
  int consistent();
};

}

#endif
