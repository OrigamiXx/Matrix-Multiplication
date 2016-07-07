/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#ifndef OINK_VARIABLE_H
#define OINK_VARIABLE_H

#include "pointers.h"

namespace OINK_NS {

class Variable : protected Pointers {
 public:
  Variable(class OINK *);
  ~Variable();
  void set(int, char **);
  void set(char *, int, char **);
  int next(int, char **);
  int find(char *);
  int equalstyle(int);
  char *retrieve(char *);
  int retrieve_count(int);
  char *retrieve_single(int, int);
  double compute_equal(int);
  double evaluate_boolean(char *);

 private:
  int me;
  int nvar;                // # of defined variables
  int maxvar;              // max # of variables arrays can hold
  char **names;            // name of each variable
  int *style;              // style of each variable
  int *num;                // # of values for each variable
  int *which;              // next available value for each variable
  int *offset;             // offset for loop variables specified as N1 to N2
  int *pad;                // 1 = pad loop/uloop variables with 0s, 0 = no pad
  char ***data;            // str value of each variable's values
  double PI;

  class RanMars *randomequal;   // random number generator for equal-style vars

  int precedence[16];      // precedence level of math operators
                           // set length to include OR in enum
  void remove(int);
  void extend();
  void copy(int, char **, char **);
  double evaluate(char *);
  int find_matching_paren(char *, int, char *&);
  int math_function(char *, char *, double *, int &);
  int is_constant(char *);
  double constant(char *);
  int is_keyword(char *);
  double keyword(char *);
  double numeric(char *);
  int inumeric(char *);
};

}

#endif
