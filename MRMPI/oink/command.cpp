/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "command.h"
#include "object.h"
#include "error.h"

using namespace OINK_NS;

/* ---------------------------------------------------------------------- */

Command::Command(OINK *oink) : Pointers(oink) {}

/* ---------------------------------------------------------------------- */

void Command::inputs(int narg, char **arg)
{
  if (narg != ninputs) error->all("Mismatch in command inputs");
  obj->setup_inputs(ninputs);
  for (int i = 0; i < narg; i++)
    obj->add_input(i,arg[i]);
}

/* ---------------------------------------------------------------------- */

void Command::outputs(int narg, char **arg)
{
  if (narg/2 != noutputs) error->all("Mismatch in command outputs");
  if (narg % 2) error->all("Command outputs must be specified in pairs");
  obj->setup_outputs(noutputs);
  for (int i = 0; i < narg; i += 2)
    obj->add_output(i/2,arg[i],arg[i+1]);
}
