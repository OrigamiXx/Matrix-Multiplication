/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "mpi.h"
#include "oink.h"
#include "input.h"

using namespace OINK_NS;

/* ----------------------------------------------------------------------
   main program to drive OINK
------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
  MPI_Init(&argc,&argv);

  OINK *oink = new OINK(argc,argv,MPI_COMM_WORLD);
  oink->input->file();
  delete oink;

  MPI_Finalize();
}
