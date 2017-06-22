/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(vertex_extract,VertexExtract)

#else

#ifndef OINK_VERTEX_EXTRACT_H
#define OINK_VERTEX_EXTRACT_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class VertexExtract : public Command {
 public:
  VertexExtract(class OINK *);
  void run();
  void params(int, char **);
};

}

#endif
#endif
