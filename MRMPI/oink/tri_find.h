/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(tri_find,TriFind)

#else

#ifndef OINK_TRI_FIND_H
#define OINK_TRI_FIND_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class TriFind : public Command {
 public:
  TriFind(class OINK *);
  void run();
  void params(int, char **);

 private:
  static void print(char *, int, char *, int, void *);

  static void map_edge_vert(uint64_t, char *, int, char *, int,
			    KeyValue *, void *);
  static void reduce_first_degree(char *, int, char *, int, int *, 
				  KeyValue *, void *);
  static void reduce_second_degree(char *, int, char *, int, int *, 
				   KeyValue *, void *);
  static void map_low_degree(uint64_t, char *, int, char *, int,
			     KeyValue *, void *);
  static void reduce_nsq_angles(char *, int, char *, int, int *, 
				KeyValue *, void *);
  static void reduce_emit_triangles(char *, int, char *, int, int *, 
				    KeyValue *, void *);
};

}

#endif
#endif
