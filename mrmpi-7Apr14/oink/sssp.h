/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(sssp,SSSP)

#else

#ifndef OINK_SSSP_H
#define OINK_SSSP_H

#include "command.h"
#include "keyvalue.h"
#include "float.h"
#include "typedefs.h"
#include <vector>

using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

typedef void REDUCE1_FN(char*, int, char*, int, int*, KeyValue*, void*);
typedef void REDUCE2_FN(char*, int, char*, int, void*);
typedef void MAP1_FN(int, KeyValue*, void*);
typedef void MAP2_FN(uint64_t, char*, int, char*, int, KeyValue*, void*);

//////////////////////////////////////////////////////////////////////////
class EDGEVALUE{
// Edge with destination vertex (8-bytes) and edge weight
// Given a key that is a VERTEX Vi, EDGEVALUE contains Vj and the weight
// of edge Vi->Vj.
public:
  VERTEX v;
  WEIGHT wt;
  friend bool operator!=(const EDGEVALUE& lhs, const EDGEVALUE& rhs) {
    if ((lhs.wt != rhs.wt) || (lhs.v != rhs.v)) return true;
    return false;
  };
};

class DISTANCE {
// Class used to pass distance information through the MapReduce system.
public:
  DISTANCE(){
    memset(&(e.v), 0, sizeof(VERTEX));
    e.wt = FLT_MAX;
    current = true;
  };
  ~DISTANCE(){};
  EDGEVALUE e;   // Edge describing the distance of a vtx from S; 
                 // e.v is predecessor vtx; e.wt is distance from S through e.v.
  bool current;  // Flag indicating that this distance is the current state
                 // for the vtx (the currently accepted best distance).
                 // Needed so we can know when to stop (when no vtx distances
                 // change in an iteration).
  friend bool operator!=(const DISTANCE& lhs,
                         const DISTANCE& rhs) {
    if (lhs.e != rhs.e) return true;
    return false;
  };
};

//////////////////////////////////////////////////////////////////////////
class SSSP : public Command {
 public:
  SSSP(class OINK *);
  void run();
  void params(int, char **);
  int me;
  int np;

 private:
  int ncnt;  // Number of SSSP computations to do.
  int seed;  // Random seed initialization
  static uint64_t NVtxLabeled;   // Number of local vertices labeled so far.

  std::vector<VERTEX> sourcelist; // ncnt sources with outdegree > zero.

  bool get_next_source(VERTEX *, int);

  static MAP1_FN add_source;
  static MAP2_FN reorganize_edges;
  static MAP2_FN move_to_new_mr;
  static MAP2_FN initialize_vertex_distances;
  static REDUCE1_FN get_good_sources;
  static REDUCE1_FN pick_shortest_distances;
  static REDUCE1_FN update_adjacent_distances;
  static REDUCE2_FN print;
};

}

#endif
#endif
