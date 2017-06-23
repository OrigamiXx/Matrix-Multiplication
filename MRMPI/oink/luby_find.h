/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(luby_find,LubyFind)

#else

#ifndef OINK_LUBY_FIND_H
#define OINK_LUBY_FIND_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class LubyFind : public Command {
 public:
  LubyFind(class OINK *);
  void run();
  void params(int, char **);

 private:
  int seed;

  static void print(char *, int, char *, int, void *);

  static void map_vert_random(uint64_t, char *, int, char *, int,
			      KeyValue *, void *);
  static void reduce_edge_winner(char *, int, char *, int, int *, 
				 KeyValue *, void *);
  static void reduce_vert_winner(char *, int, char *, int, int *, 
				 KeyValue *, void *);
  static void reduce_vert_loser(char *, int, char *, int, int *, 
				KeyValue *, void *);
  static void reduce_vert_emit(char *, int, char *, int, int *, 
			       KeyValue *, void *);
};

}

#endif
#endif
