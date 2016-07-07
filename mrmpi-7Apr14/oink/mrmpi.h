/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_MRMPI_H
#define OINK_MRMPI_H

#include "typedefs.h"
#include "pointers.h"

namespace OINK_NS {

class MRMPI : protected Pointers {
 public:
  MRMPI(class OINK *);
  ~MRMPI() {}
  void run(int, int, char **);

  HashFnPtr hash_lookup(char *);
  CompareFnPtr compare_lookup(char *);
  MapTaskFnPtr map_task_lookup(char *);
  MapFileFnPtr map_file_lookup(char *);
  MapStringFnPtr map_string_lookup(char *);
  MapMRFnPtr map_mr_lookup(char *);
  ReduceFnPtr reduce_lookup(char *);
  ScanKVFnPtr scan_kv_lookup(char *);
  ScanKMVFnPtr scan_kmv_lookup(char *);
};

}

#endif
