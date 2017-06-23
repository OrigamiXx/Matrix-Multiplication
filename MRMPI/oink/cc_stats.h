/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(cc_stats,CCStats)

#else

#ifndef OINK_CC_STATS_H
#define OINK_CC_STATS_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class CCStats : public Command {
 public:
  CCStats(class OINK *);
  void run();
  void params(int, char **);

 private:
  static void read(int, char *, KeyValue *kv, void *);
  static void print(char *, int, char *, int, void *);

  static void map_invert(uint64_t, char *, int, char *, int,
			 KeyValue *, void *);
  static void reduce_sum(char *, int, char *, int, int *, 
			 KeyValue *, void *);
  static int compare_uint64(char *, int, char *, int);
  static void map_print(uint64_t, char *, int, char *, int,
			KeyValue *, void *);
};

}

#endif
#endif
