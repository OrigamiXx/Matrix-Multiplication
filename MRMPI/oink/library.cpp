/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

// C or Fortran style library interface to OINK
// customize by adding new OINK-specific functions

#include "mpi.h"
#include "string.h"
#include "stdlib.h"
#include "library.h"
#include "oink.h"
#include "input.h"

using namespace OINK_NS;

/* ----------------------------------------------------------------------
   create an instance of OINK and return pointer to it
   pass in command-line args and MPI communicator to run on
------------------------------------------------------------------------- */

void oink_open(int argc, char **argv, MPI_Comm communicator, void **ptr)
{
  OINK *oink = new OINK(argc,argv,communicator);
  *ptr = (void *) oink;
}

/* ----------------------------------------------------------------------
   create an instance of OINK and return pointer to it
   caller doesn't know MPI communicator, so use MPI_COMM_WORLD
   intialize MPI if needed
------------------------------------------------------------------------- */

void oink_open_no_mpi(int argc, char **argv, void **ptr)
{
  int flag;
  MPI_Initialized(&flag);

  if (!flag) {
    int argc = 0;
    char **argv = NULL;
    MPI_Init(&argc,&argv);
  }

  MPI_Comm communicator = MPI_COMM_WORLD;

  OINK *oink = new OINK(argc,argv,communicator);
  *ptr = (void *) oink;
}

/* ----------------------------------------------------------------------
   destruct an instance of OINK
------------------------------------------------------------------------- */

void oink_close(void *ptr)
{
  OINK *oink = (OINK *) ptr;
  delete oink;
}

/* ----------------------------------------------------------------------
   process an input script in filename str
------------------------------------------------------------------------- */

void oink_file(void *ptr, char *str)
{
  OINK *oink = (OINK *) ptr;
  oink->input->file(str);
}

/* ----------------------------------------------------------------------
   process a single input command in str
------------------------------------------------------------------------- */

char *oink_command(void *ptr, char *str)
{
  OINK *oink = (OINK *) ptr;
  return oink->input->one(str);
}

/* ----------------------------------------------------------------------
   clean-up function to free memory allocated by lib and returned to caller
------------------------------------------------------------------------- */

void oink_free(void *ptr)
{
  free(ptr);
}

/* ----------------------------------------------------------------------
   add OINK-specific library functions
   all must receive OINK pointer as argument
   customize by adding a function here and in library.h header file
------------------------------------------------------------------------- */
