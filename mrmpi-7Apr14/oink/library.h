/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

/* 
   C or Fortran style library interface to MRMPI
   new MRMPI-specific functions can be added
*/

#include "mpi.h"

/* ifdefs allow this file to be included in a C program */

#ifdef __cplusplus
extern "C" {
#endif

void mrmpi_open(int, char **, MPI_Comm, void **);
void mrmpi_open_no_mpi(int, char **, void **);
void mrmpi_close(void *);
void mrmpi_file(void *, char *);
char *mrmpi_command(void *, char *);
void mrmpi_free(void *);

#ifdef __cplusplus
}
#endif
