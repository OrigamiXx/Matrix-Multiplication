/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(cc_find,CCFind)

#else

#ifndef OINK_CC_FIND_H
#define OINK_CC_FIND_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class CCFind : public Command {
 public:
  CCFind(class OINK *);
  void run();
  void params(int, char **);

 private:
  int me,nprocs;
  int seed,nthresh;
  uint64_t nvert;
  int flag,pshift;
  uint64_t lmask;
  
  typedef struct {
    uint64_t zone,empty;
  } PAD;
  PAD pad;

  static void print(char *, int, char *, int, void *);

  static void reduce_self_zone(char *, int, char *, int, int *, 
			       KeyValue *, void *);
  static void map_edge_vert(uint64_t, char *, int, char *, int,
			    KeyValue *, void *);
  static void reduce_edge_zone(char *, int, char *, int, int *, 
			       KeyValue *, void *);
  static void reduce_zone_winner(char *, int, char *, int, int *, 
				 KeyValue *, void *);
  static void map_invert_multi(uint64_t, char *, int, char *, int,
			       KeyValue *, void *);
  static void map_zone_multi(uint64_t, char *, int, char *, int,
			     KeyValue *, void *);
  static void reduce_zone_reassign(char *, int, char *, int, int *, 
				   KeyValue *, void *);
  static void map_strip(uint64_t, char *, int, char *, int,
			KeyValue *, void *);
};

}

#endif
#endif
