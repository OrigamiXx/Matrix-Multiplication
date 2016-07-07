/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifdef COMMAND_CLASS

CommandStyle(wordfreq,WordFreq)

#else

#ifndef OINK_WORD_FREQ_H
#define OINK_WORD_FREQ_H

#include "command.h"
#include "keyvalue.h"
using MAPREDUCE_NS::KeyValue;

namespace OINK_NS {

class WordFreq : public Command {
 public:
  WordFreq(class OINK *);
  void run();
  void params(int, char **);

 private:
  int ntop;

  static void output(uint64_t, char *, int, char *, int, KeyValue *, void *);
};

}

#endif
#endif
