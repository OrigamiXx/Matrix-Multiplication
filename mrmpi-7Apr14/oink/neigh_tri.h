/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(neigh_tri,NeighTri)

#else

#ifndef OINK_NEIGH_TRI_H
#define OINK_NEIGH_TRI_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class NeighTri : public Command {
 public:
  NeighTri(class OINK *);
  void run();
  void params(int, char **);

 private:
  char *dirname;

  static void nread(int, char *, KeyValue *, void *);
  static void tread(int, char *, KeyValue *, void *);
  static void print(char *, int, char *, int, int *, void *);
  static void map1(uint64_t, char *, int, char *, int, KeyValue *, void *);
};

}

#endif
#endif
