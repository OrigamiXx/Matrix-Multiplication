/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_MEMORY_H
#define OINK_MEMORY_H

#include "pointers.h"

namespace OINK_NS {

class Memory : protected Pointers {
 public:
  Memory(class OINK *);

  void *smalloc(int n, const char *);
  void sfree(void *);
  void *srealloc(void *, int n, const char *);

  double *create_1d_double_array(int, int, const char *);
  void destroy_1d_double_array(double *, int);
  
  double **create_2d_double_array(int, int, const char *);
  void destroy_2d_double_array(double **);
  double **grow_2d_double_array(double **, int, int, const char *);

  int **create_2d_int_array(int, int, const char *);
  void destroy_2d_int_array(int **);
  int **grow_2d_int_array(int **, int, int, const char *);

  double **create_2d_double_array(int, int, int, const char *);
  void destroy_2d_double_array(double **, int);

  double ***create_3d_double_array(int, int, int, const char *);
  void destroy_3d_double_array(double ***);
  double ***grow_3d_double_array(double ***, int, int, int, const char *);

  double ***create_3d_double_array(int, int, int, int, const char *);
  void destroy_3d_double_array(double ***, int);

  double ***create_3d_double_array(int, int, int, int, int, int, const char *);
  void destroy_3d_double_array(double ***, int, int, int);

  int ***create_3d_int_array(int, int, int, const char *);
  void destroy_3d_int_array(int ***);

  double ****create_4d_double_array(int, int, int, int, const char *);
  void destroy_4d_double_array(double ****);
};

}

#endif
