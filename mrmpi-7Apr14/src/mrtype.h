/* ----------------------------------------------------------------------
   MR-MPI = MapReduce-MPI library
   http://www.cs.sandia.gov/~sjplimp/mapreduce.html
   Steve Plimpton, sjplimp@sandia.gov, Sandia National Laboratories

   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the modified Berkeley Software Distribution (BSD) License.

   See the README file in the top-level MapReduce directory.
------------------------------------------------------------------------- */

#ifndef MRTYPE_H
#define MRTYPE_H

#include "mpi.h"
#include "stdint.h"

namespace MAPREDUCE_NS {

// MRMPI_BIGINT must be set to what MPI treats as 8-byte unsigned int

#define MRMPI_BIGINT MPI_UNSIGNED_LONG_LONG
//#define MRMPI_BIGINT MPI_UNSIGNED_LONG

}

#endif
