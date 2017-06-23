/* ----------------------------------------------------------------------
   OINK - scripting wrapper on MapReduce-MPI library
   http://www.sandia.gov/~sjplimp/mapreduce.html, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   See the README file in the top-level MR-MPI directory.
------------------------------------------------------------------------- */

#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "unistd.h"
#include "variable.h"
#include "universe.h"
#include "input.h"
#include "random_mars.h"
#include "memory.h"
#include "error.h"

using namespace OINK_NS;

#define VARDELTA 4
#define MAXLEVEL 4

#define MIN(A,B) ((A) < (B)) ? (A) : (B)
#define MAX(A,B) ((A) > (B)) ? (A) : (B)

#define MYROUND(a) (( a-floor(a) ) >= .5) ? ceil(a) : floor(a)

enum{INDEX,LOOP,WORLD,UNIVERSE,ULOOP,STRING,EQUAL};
enum{ARG,OP};

// customize by adding a function

enum{DONE,ADD,SUBTRACT,MULTIPLY,DIVIDE,CARAT,UNARY,
       NOT,EQ,NE,LT,LE,GT,GE,AND,OR,
       SQRT,EXP,LN,LOG,SIN,COS,TAN,ASIN,ACOS,ATAN,ATAN2,
       RANDOM,NORMAL,CEIL,FLOOR,ROUND};

#define BIG 1.0e20

/* ---------------------------------------------------------------------- */

Variable::Variable(OINK *oink) : Pointers(oink)
{
  MPI_Comm_rank(world,&me);

  nvar = maxvar = 0;
  names = NULL;
  style = NULL;
  num = NULL;
  which = NULL;
  offset = NULL;
  pad = NULL;
  data = NULL;

  randomequal = NULL;

  precedence[DONE] = 0;
  precedence[OR] = 1;
  precedence[AND] = 2;
  precedence[EQ] = precedence[NE] = 3;
  precedence[LT] = precedence[LE] = precedence[GT] = precedence[GE] = 4;
  precedence[ADD] = precedence[SUBTRACT] = 5;
  precedence[MULTIPLY] = precedence[DIVIDE] = 6;
  precedence[CARAT] = 7;
  precedence[UNARY] = precedence[NOT] = 8;

  PI = 4.0*atan(1.0);
}

/* ---------------------------------------------------------------------- */

Variable::~Variable()
{
  for (int i = 0; i < nvar; i++) {
    delete [] names[i];
    if (style[i] == LOOP || style[i] == ULOOP) delete [] data[i][0];
    else for (int j = 0; j < num[i]; j++) delete [] data[i][j];
    delete [] data[i];
  }
  memory->sfree(names);
  memory->sfree(style);
  memory->sfree(num);
  memory->sfree(which);
  memory->sfree(offset);
  memory->sfree(pad);
  memory->sfree(data);

  delete randomequal;
}

/* ----------------------------------------------------------------------
   called by variable command in input script
------------------------------------------------------------------------- */

void Variable::set(int narg, char **arg)
{
  if (narg < 2) error->all("Illegal variable command");

  // DELETE
  // doesn't matter if variable no longer exists

  if (strcmp(arg[1],"delete") == 0) {
    if (narg != 2) error->all("Illegal variable command");
    if (find(arg[0]) >= 0) remove(find(arg[0]));
    return;

  // INDEX
  // num = listed args, which = 1st value, data = copied args

  } else if (strcmp(arg[1],"index") == 0) {
    if (narg < 3) error->all("Illegal variable command");
    if (find(arg[0]) >= 0) return;
    if (nvar == maxvar) extend();
    style[nvar] = INDEX;
    num[nvar] = narg - 2;
    which[nvar] = 0;
    offset[nvar] = 0;
    pad[nvar] = 0;
    data[nvar] = new char*[num[nvar]];
    copy(num[nvar],&arg[2],data[nvar]);

  // LOOP
  // 1 arg + pad: num = N, which = offset = 1st value, data = single string
  // 2 args + pad: same but num = N2-N1+1, offset = N1

  } else if (strcmp(arg[1],"loop") == 0) {
    if (find(arg[0]) >= 0) return;
    if (nvar == maxvar) extend();
    style[nvar] = LOOP;
    int nfirst,nlast;
    if (narg == 3 || (narg == 4 && strcmp(arg[3],"pad") == 0)) {
      nfirst = 1;
      nlast = atoi(arg[2]);
      if (nlast <= 0) error->all("Illegal variable command");
      if (narg == 4 && strcmp(arg[3],"pad") == 0) {
	char digits[12];
	sprintf(digits,"%d",nlast);
	pad[nvar] = strlen(digits);
      } else pad[nvar] = 0;
    } else if (narg == 4 || (narg == 5 && strcmp(arg[4],"pad") == 0)) {
      nfirst = atoi(arg[2]);
      nlast = atoi(arg[3]);
      if (nfirst > nlast || nlast <= 0) error->all("Illegal variable command");
      if (narg == 5 && strcmp(arg[4],"pad") == 0) {
	char digits[12];
	sprintf(digits,"%d",nlast);
	pad[nvar] = strlen(digits);
      } else pad[nvar] = 0;
    } else error->all("Illegal variable command");
    num[nvar] = nlast-nfirst+1;
    which[nvar] = 0;
    offset[nvar] = nfirst;
    data[nvar] = new char*[1];
    data[nvar][0] = NULL;

  // WORLD
  // num = listed args, which = partition this proc is in, data = copied args
  // error check that num = # of worlds in universe

  } else if (strcmp(arg[1],"world") == 0) {
    if (narg < 3) error->all("Illegal variable command");
    if (find(arg[0]) >= 0) return;
    if (nvar == maxvar) extend();
    style[nvar] = WORLD;
    num[nvar] = narg - 2;
    if (num[nvar] != universe->nworlds)
      error->all("World variable count doesn't match # of partitions");
    which[nvar] = universe->iworld;
    offset[nvar] = 0;
    pad[nvar] = 0;
    data[nvar] = new char*[num[nvar]];
    copy(num[nvar],&arg[2],data[nvar]);

  // UNIVERSE and ULOOP
  // for UNIVERSE: num = listed args, data = copied args
  // for ULOOP: num = N, data = single string
  // which = partition this proc is in
  // universe proc 0 creates lock file
  // error check that all other universe/uloop variables are same length

  } else if (strcmp(arg[1],"universe") == 0 || strcmp(arg[1],"uloop") == 0) {
    if (strcmp(arg[1],"universe") == 0) {
      if (narg < 3) error->all("Illegal variable command");
      if (find(arg[0]) >= 0) return;
      if (nvar == maxvar) extend();
      style[nvar] = UNIVERSE;
      num[nvar] = narg - 2;
      data[nvar] = new char*[num[nvar]];
      copy(num[nvar],&arg[2],data[nvar]);
    } else if (strcmp(arg[1],"uloop") == 0) {
      if (narg < 3 || narg > 4 || (narg == 4 && strcmp(arg[3],"pad") != 0))
	error->all("Illegal variable command");
      if (find(arg[0]) >= 0) return;
      if (nvar == maxvar) extend();
      style[nvar] = ULOOP;
      num[nvar] = atoi(arg[2]);
      offset[nvar] = 0;
      data[nvar] = new char*[1];
      data[nvar][0] = NULL;
      if (narg == 4) {
	char digits[12];
	sprintf(digits,"%d",num[nvar]);
	pad[nvar] = strlen(digits);
      } else pad[nvar] = 0;
    }

    if (num[nvar] < universe->nworlds)
      error->all("Universe/uloop variable count < # of partitions");
    which[nvar] = universe->iworld;

    if (universe->me == 0) {
      FILE *fp = fopen("tmp.oink.variable","w");
      fprintf(fp,"%d\n",universe->nworlds);
      fclose(fp);
    }

    for (int jvar = 0; jvar < nvar; jvar++)
      if (num[jvar] && (style[jvar] == UNIVERSE || style[jvar] == ULOOP) && 
	  num[nvar] != num[jvar])
	error->all("All universe/uloop variables must have same # of values");

  // STRING
  // remove pre-existing var if also style STRING (allows it to be reset)
  // num = 1, which = 1st value
  // data = 1 value, string to eval

  } else if (strcmp(arg[1],"string") == 0) {
    if (narg != 3) error->all("Illegal variable command");
    if (find(arg[0]) >= 0) {
      if (style[find(arg[0])] != STRING)
	error->all("Cannot redefine variable as a different style");
      remove(find(arg[0]));
    }
    if (nvar == maxvar) extend();
    style[nvar] = STRING;
    num[nvar] = 1;
    which[nvar] = 0;
    offset[nvar] = 0;
    pad[nvar] = 0;
    data[nvar] = new char*[num[nvar]];
    copy(1,&arg[2],data[nvar]);
    
  // EQUAL
  // remove pre-existing var if also style EQUAL (allows it to be reset)
  // num = 2, which = 1st value
  // data = 2 values, 1st is string to eval, 2nd is filled on retrieval

  } else if (strcmp(arg[1],"equal") == 0) {
    if (narg != 3) error->all("Illegal variable command");
    if (find(arg[0]) >= 0) {
      if (style[find(arg[0])] != EQUAL)
	error->all("Cannot redefine variable as a different style");
      remove(find(arg[0]));
    }
    if (nvar == maxvar) extend();
    style[nvar] = EQUAL;
    num[nvar] = 2;
    which[nvar] = 0;
    offset[nvar] = 0;
    pad[nvar] = 0;
    data[nvar] = new char*[num[nvar]];
    copy(1,&arg[2],data[nvar]);
    data[nvar][1] = NULL;

  } else error->all("Illegal variable command");

  // set name of variable
  // must come at end, since STRING/EQUAL/ATOM reset may have removed name
  // name must be all alphanumeric chars or underscores

  int n = strlen(arg[0]) + 1;
  names[nvar] = new char[n];
  strcpy(names[nvar],arg[0]);

  for (int i = 0; i < n-1; i++)
    if (!isalnum(names[nvar][i]) && names[nvar][i] != '_')
      error->all("Variable name must be alphanumeric or "
		 "underscore characters");
  nvar++;
}

/* ----------------------------------------------------------------------
   INDEX variable created by command-line argument
   make it INDEX rather than STRING so cannot be re-defined in input script
------------------------------------------------------------------------- */

void Variable::set(char *name, int narg, char **arg)
{
  char **newarg = new char*[2+narg];
  newarg[0] = name;
  newarg[1] = (char *) "index";
  for (int i = 0; i < narg; i++) newarg[2+i] = arg[i];
  set(2+narg,newarg);
  delete [] newarg;
}

/* ----------------------------------------------------------------------
   increment variable(s)
   return 0 if OK if successfully incremented
   return 1 if any variable is exhausted, free the variable to allow re-use
------------------------------------------------------------------------- */

int Variable::next(int narg, char **arg)
{
  int ivar;

  if (narg == 0) error->all("Illegal next command");

  // check that variables exist and are all the same style
  // exception: UNIVERSE and ULOOP variables can be mixed in same next command

  for (int iarg = 0; iarg < narg; iarg++) {
    ivar = find(arg[iarg]);
    if (ivar == -1) error->all("Invalid variable in next command");
    if (style[ivar] == ULOOP && style[find(arg[0])] == UNIVERSE) continue;
    else if (style[ivar] == UNIVERSE && style[find(arg[0])] == ULOOP) continue;
    else if (style[ivar] != style[find(arg[0])])
      error->all("All variables in next command must be same style");
  }

  // invalid styles STRING or EQUAL or WORLD

  int istyle = style[find(arg[0])];
  if (istyle == STRING || istyle == EQUAL || istyle == WORLD)
    error->all("Invalid variable style with next command");

  // increment all variables in list
  // if any variable is exhausted, set flag = 1 and remove var to allow re-use

  int flag = 0;

  if (istyle == INDEX || istyle == LOOP) {
    for (int iarg = 0; iarg < narg; iarg++) {
      ivar = find(arg[iarg]);
      which[ivar]++;
      if (which[ivar] >= num[ivar]) {
	flag = 1;
	remove(ivar);
      }
    }

  } else if (istyle == UNIVERSE || istyle == ULOOP) {

    // wait until lock file can be created and owned by proc 0 of this world
    // read next available index and Bcast it within my world
    // set all variables in list to nextindex

    int nextindex;
    if (me == 0) {
      while (1) {
	if (!rename("tmp.oink.variable","tmp.oink.variable.lock")) break;
	usleep(100000);
      }
      FILE *fp = fopen("tmp.oink.variable.lock","r");
      fscanf(fp,"%d",&nextindex);
      fclose(fp);
      fp = fopen("tmp.oink.variable.lock","w");
      fprintf(fp,"%d\n",nextindex+1);
      fclose(fp);
      rename("tmp.oink.variable.lock","tmp.oink.variable");
      if (universe->uscreen)
	fprintf(universe->uscreen,
		"Increment via next: value %d on partition %d\n",
		nextindex+1,universe->iworld);
      if (universe->ulogfile)
	fprintf(universe->ulogfile,
		"Increment via next: value %d on partition %d\n",
		nextindex+1,universe->iworld);
    }
    MPI_Bcast(&nextindex,1,MPI_INT,0,world);

    for (int iarg = 0; iarg < narg; iarg++) {
      ivar = find(arg[iarg]);
      which[ivar] = nextindex;
      if (which[ivar] >= num[ivar]) {
	flag = 1;
	remove(ivar);
      }
    }
  }

  return flag;
}

/* ----------------------------------------------------------------------
   return ptr to the data text associated with a variable
   if INDEX or WORLD or UNIVERSE or STRING var, return ptr to stored string
   if LOOP or ULOOP var, write int to data[0] and return ptr to string
   if EQUAL var, evaluate variable and put result in str
   return NULL if no variable or which is bad, caller must respond
------------------------------------------------------------------------- */

char *Variable::retrieve(char *name)
{
  int ivar = find(name);
  if (ivar == -1) return NULL;
  if (which[ivar] >= num[ivar]) return NULL;

  if (style[ivar] == INDEX || style[ivar] == WORLD || 
      style[ivar] == UNIVERSE || style[ivar] == STRING)
    return data[ivar][which[ivar]];

  if (style[ivar] == LOOP || style[ivar] == ULOOP) {
    char result[16];
    if (pad[ivar] == 0) sprintf(result,"%d",which[ivar]+offset[ivar]);
    else {
      char padstr[16];
      sprintf(padstr,"%%0%dd",pad[ivar]);
      sprintf(result,padstr,which[ivar]+offset[ivar]);
    }
    int n = strlen(result) + 1;
    delete [] data[ivar][0];
    data[ivar][0] = new char[n];
    strcpy(data[ivar][0],result);
    return data[ivar][0];
  }

  if (style[ivar] == EQUAL) {
    char result[32];
    double answer = evaluate(data[ivar][0]);
    sprintf(result,"%.10g",answer);
    int n = strlen(result) + 1;
    if (data[ivar][1]) delete [] data[ivar][1];
    data[ivar][1] = new char[n];
    strcpy(data[ivar][1],result);
    return data[ivar][1];
  }

  return NULL;
}

/* ----------------------------------------------------------------------
   return count of data strings associated with variable ivar
   should not be called for EQUAL var
------------------------------------------------------------------------- */

int Variable::retrieve_count(int ivar)
{
  return num[ivar];
}

/* ----------------------------------------------------------------------
   return Nth data string associated with variable ivar
   incrementing is NOT performed
   caller insures N is within proper range 0 to num-1
   should not be called for EQUAL var
------------------------------------------------------------------------- */

char *Variable::retrieve_single(int ivar, int nth)
{
  if (style[ivar] == INDEX || style[ivar] == WORLD || 
      style[ivar] == UNIVERSE || style[ivar] == STRING)
    return data[ivar][nth];

  // style LOOP or ULOOP

  char result[16];
  if (pad[ivar] == 0) sprintf(result,"%d",nth+offset[ivar]);
  else {
    char padstr[16];
    sprintf(padstr,"%%0%dd",pad[ivar]);
    sprintf(result,padstr,nth+offset[ivar]);
  }
  int n = strlen(result) + 1;
  delete [] data[ivar][0];
  data[ivar][0] = new char[n];
  strcpy(data[ivar][0],result);
  return data[ivar][0];
}

/* ----------------------------------------------------------------------
   return result of equal-style variable evaluation
------------------------------------------------------------------------- */

double Variable::compute_equal(int ivar)
{
  return evaluate(data[ivar][0]);
}

/* ----------------------------------------------------------------------
   search for name in list of variables names
   return index or -1 if not found
------------------------------------------------------------------------- */
  
int Variable::find(char *name)
{
  for (int i = 0; i < nvar; i++)
    if (strcmp(name,names[i]) == 0) return i;
  return -1;
}

/* ----------------------------------------------------------------------
   return 1 if variable is EQUAL style, 0 if not
------------------------------------------------------------------------- */
  
int Variable::equalstyle(int ivar)
{
  if (style[ivar] == EQUAL) return 1;
  return 0;
}

/* ----------------------------------------------------------------------
   remove Nth variable from list and compact list
------------------------------------------------------------------------- */
  
void Variable::remove(int n)
{
  delete [] names[n];
  if (style[n] == LOOP || style[n] == ULOOP) delete [] data[n][0];
  else for (int i = 0; i < num[n]; i++) delete [] data[n][i];
  delete [] data[n];

  for (int i = n+1; i < nvar; i++) {
    names[i-1] = names[i];
    style[i-1] = style[i];
    num[i-1] = num[i];
    which[i-1] = which[i];
    offset[i-1] = offset[i];
    pad[i-1] = pad[i];
    data[i-1] = data[i];
  }
  nvar--;
}

/* ----------------------------------------------------------------------
  make space in arrays for new variable
------------------------------------------------------------------------- */

void Variable::extend()
{
  maxvar += VARDELTA;
  names = (char **)
    memory->srealloc(names,maxvar*sizeof(char *),"var:names");
  style = (int *) memory->srealloc(style,maxvar*sizeof(int),"var:style");
  num = (int *) memory->srealloc(num,maxvar*sizeof(int),"var:num");
  which = (int *) memory->srealloc(which,maxvar*sizeof(int),"var:which");
  offset = (int *) memory->srealloc(offset,maxvar*sizeof(int),"var:offset");
  pad = (int *) memory->srealloc(pad,maxvar*sizeof(int),"var:pad");
  data = (char ***) 
    memory->srealloc(data,maxvar*sizeof(char **),"var:data");
}

/* ----------------------------------------------------------------------
   copy narg strings from **from to **to, and allocate space for them
------------------------------------------------------------------------- */
  
void Variable::copy(int narg, char **from, char **to)
{
  int n;
  for (int i = 0; i < narg; i++) {
    n = strlen(from[i]) + 1;
    to[i] = new char[n];
    strcpy(to[i],from[i]);
  }
}

/* ----------------------------------------------------------------------
   recursive evaluation of a string str
   str is an equal-style or atom-style formula containing one or more items:
     number = 0.0, -5.45, 2.8e-4, ...
     constant = PI
     keyword = nprocs, time
     math operation = (),-x,x+y,x-y,x*y,x/y,x^y,
                      x==y,x!=y,x<y,x<=y,x>y,x>=y,x&&y,x||y,
                      sqrt(x),exp(x),ln(x),log(x),
		      sin(x),cos(x),tan(x),asin(x),atan2(y,x),...
     variable = v_name, v_name[i]
   evaluate the formula, return result as a double
------------------------------------------------------------------------- */

double Variable::evaluate(char *str)
{
  int op,opprevious;
  double value1,value2;
  char onechar;
  char *ptr;

  double argstack[MAXLEVEL];
  int opstack[MAXLEVEL];
  int nargstack = 0;
  int nopstack = 0;

  int i = 0;
  int expect = ARG;

  while (1) {
    onechar = str[i];

    // whitespace: just skip

    if (isspace(onechar)) i++;

    // ----------------
    // parentheses: recursively evaluate contents of parens
    // ----------------

    else if (onechar == '(') {
      if (expect == OP) error->all("Invalid syntax in variable formula");
      expect = OP;

      char *contents;
      i = find_matching_paren(str,i,contents);
      i++;

      // evaluate contents and push on stack

      argstack[nargstack++] = evaluate(contents);

      delete [] contents;

    // ----------------
    // number: push value onto stack
    // ----------------

    } else if (isdigit(onechar) || onechar == '.') {
      if (expect == OP) error->all("Invalid syntax in variable formula");
      expect = OP;

      // istop = end of number, including scientific notation

      int istart = i;
      while (isdigit(str[i]) || str[i] == '.') i++;
      if (str[i] == 'e' || str[i] == 'E') {
	i++;
	if (str[i] == '+' || str[i] == '-') i++;
	while (isdigit(str[i])) i++;
      }
      int istop = i - 1;

      int n = istop - istart + 1;
      char *number = new char[n+1];
      strncpy(number,&str[istart],n);
      number[n] = '\0';

      argstack[nargstack++] = atof(number);

      delete [] number;

    // ----------------
    // letter: v_name, exp(), PI, keyword
    // ----------------

    } else if (isalpha(onechar)) {
      if (expect == OP) error->all("Invalid syntax in variable formula");
      expect = OP;

      // istop = end of word
      // word = all alphanumeric or underscore

      int istart = i;
      while (isalnum(str[i]) || str[i] == '_') i++;
      int istop = i-1;

      int n = istop - istart + 1;
      char *word = new char[n+1];
      strncpy(word,&str[istart],n);
      word[n] = '\0';

      // ----------------
      // variable
      // ----------------

      if (strncmp(word,"v_",2) == 0) {
	n = strlen(word) - 2 + 1;
	char *id = new char[n];
	strcpy(id,&word[2]);

	int ivar = find(id);
	if (ivar < 0) error->all("Invalid variable name in variable formula");

        // v_name = scalar from non atom-style global scalar

	char *var = retrieve(id);
	if (var == NULL)
	  error->all("Invalid variable evaluation in variable formula");
	argstack[nargstack++] = atof(var);
	
	delete [] id;

      // ----------------
      // math function or constant or keyword
      // ----------------

      } else {

	// ----------------
	// math function
	// ----------------

	if (str[i] == '(') {
	  char *contents;
	  i = find_matching_paren(str,i,contents);
	  i++;

	  if (math_function(word,contents,argstack,nargstack));
	  else error->all("Invalid math function in variable formula");
	  delete [] contents;

	// ----------------
	// constant
	// ----------------

	} else if (is_constant(word)) {
	  value1 = constant(word);
	  argstack[nargstack++] = value1;

	// ----------------
	// keyword
	// ----------------

	} else if (is_keyword(word)) {
	  value1 = keyword(word);
	  argstack[nargstack++] = value1;
	
	} else error->all("Invalid keyword in variable formula");
      }

      delete [] word;

    // ----------------
    // math operator, including end-of-string
    // ----------------

    } else if (strchr("+-*/^<>=!&|\0",onechar)) {
      if (onechar == '+') op = ADD;
      else if (onechar == '-') op = SUBTRACT;
      else if (onechar == '*') op = MULTIPLY;
      else if (onechar == '/') op = DIVIDE;
      else if (onechar == '^') op = CARAT;
      else if (onechar == '=') {
	if (str[i+1] != '=') error->all("Invalid syntax in variable formula");
	op = EQ;
	i++;
      } else if (onechar == '!') {
	if (str[i+1] == '=') {
	  op = NE;
	  i++;
	} else op = NOT;
      } else if (onechar == '<') {
	if (str[i+1] != '=') op = LT;
	else {
	  op = LE;
	  i++;
	}
      } else if (onechar == '>') {
	if (str[i+1] != '=') op = GT;
	else {
	  op = GE;
	  i++;
	}
      } else if (onechar == '&') {
	if (str[i+1] != '&') error->all("Invalid syntax in variable formula");
	op = AND;
	i++;
      } else if (onechar == '|') {
	if (str[i+1] != '|') error->all("Invalid syntax in variable formula");
	op = OR;
	i++;
      } else op = DONE;

      i++;

      if (op == SUBTRACT && expect == ARG) {
	opstack[nopstack++] = UNARY;
	continue;
      }
      if (op == NOT && expect == ARG) {
	opstack[nopstack++] = op;
	continue;
      }

      if (expect == ARG) error->all("Invalid syntax in variable formula");
      expect = ARG;

      // evaluate stack as deep as possible while respecting precedence
      // before pushing current op onto stack

      while (nopstack && precedence[opstack[nopstack-1]] >= precedence[op]) {
	opprevious = opstack[--nopstack];

	value2 = argstack[--nargstack];
	if (opprevious != UNARY && opprevious != NOT)
	  value1 = argstack[--nargstack];
	
	if (opprevious == ADD)
	  argstack[nargstack++] = value1 + value2;
	else if (opprevious == SUBTRACT)
	  argstack[nargstack++] = value1 - value2;
	else if (opprevious == MULTIPLY)
	  argstack[nargstack++] = value1 * value2;
	else if (opprevious == DIVIDE) {
	  if (value2 == 0.0) error->all("Divide by 0 in variable formula");
	  argstack[nargstack++] = value1 / value2;
	} else if (opprevious == CARAT) {
	  if (value2 == 0.0) error->all("Power by 0 in variable formula");
	  argstack[nargstack++] = pow(value1,value2);
	} else if (opprevious == UNARY) {
	  argstack[nargstack++] = -value2;
	} else if (opprevious == NOT) {
	  if (value2 == 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == EQ) {
	  if (value1 == value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == NE) {
	  if (value1 != value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == LT) {
	  if (value1 < value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == LE) {
	  if (value1 <= value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == GT) {
	  if (value1 > value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == GE) {
	  if (value1 >= value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == AND) {
	  if (value1 != 0.0 && value2 != 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == OR) {
	  if (value1 != 0.0 || value2 != 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	}
      }

      // if end-of-string, break out of entire formula evaluation loop

      if (op == DONE) break;

      // push current operation onto stack

      opstack[nopstack++] = op;

    } else error->all("Invalid syntax in variable formula");
  }

  if (nopstack) error->all("Invalid syntax in variable formula");

  // return remaining arg

  if (nargstack != 1) error->all("Invalid syntax in variable formula");
  return argstack[0];
}

/* ----------------------------------------------------------------------
   find matching parenthesis in str, allocate contents = str between parens
   i = left paren
   return loc or right paren
------------------------------------------------------------------------- */

int Variable::find_matching_paren(char *str, int i,char *&contents)
{
  // istop = matching ')' at same level, allowing for nested parens

  int istart = i;
  int ilevel = 0;
  while (1) {
    i++;
    if (!str[i]) break;
    if (str[i] == '(') ilevel++;
    else if (str[i] == ')' && ilevel) ilevel--;
    else if (str[i] == ')') break;
  }
  if (!str[i]) error->all("Invalid syntax in variable formula");
  int istop = i;

  int n = istop - istart - 1;
  contents = new char[n+1];
  strncpy(contents,&str[istart+1],n);
  contents[n] = '\0';

  return istop;
}

/* ----------------------------------------------------------------------
   process a math function in formula
   push result onto tree or arg stack
   word = math function
   contents = str between parentheses with one,two,three args
   return 0 if not a match, 1 if successfully processed
   customize by adding a math function:
     sqrt(),exp(),ln(),log(),sin(),cos(),tan(),asin(),acos(),atan(),
     atan2(y,x),random(x,y,z),normal(x,y,z),ceil(),floor(),round()
------------------------------------------------------------------------- */

int Variable::math_function(char *word, char *contents,
			    double *argstack, int &nargstack)
{
  // word not a match to any math function

  if (strcmp(word,"sqrt") && strcmp(word,"exp") && 
      strcmp(word,"ln") && strcmp(word,"log") &&
      strcmp(word,"sin") && strcmp(word,"cos") &&
      strcmp(word,"tan") && strcmp(word,"asin") &&
      strcmp(word,"acos") && strcmp(word,"atan") && 
      strcmp(word,"atan2") && strcmp(word,"random") && 
      strcmp(word,"normal") && strcmp(word,"ceil") && 
      strcmp(word,"floor") && strcmp(word,"round"))
    return 0;

  // parse contents for arg1,arg2,arg3 separated by commas
  // ptr1,ptr2 = location of 1st and 2nd comma, NULL if none

  char *arg1,*arg2,*arg3;
  char *ptr1,*ptr2;

  ptr1 = strchr(contents,',');
  if (ptr1) {
    *ptr1 = '\0';
    ptr2 = strchr(ptr1+1,',');
    if (ptr2) *ptr2 = '\0';
  } else ptr2 = NULL;

  int n = strlen(contents) + 1;
  arg1 = new char[n];
  strcpy(arg1,contents);
  int narg = 1;
  if (ptr1) {
    n = strlen(ptr1+1) + 1;
    arg2 = new char[n];
    strcpy(arg2,ptr1+1);
    narg = 2;
  } else arg2 = NULL;
  if (ptr2) {
    n = strlen(ptr2+1) + 1;
    arg3 = new char[n];
    strcpy(arg3,ptr2+1);
    narg = 3;
  } else arg3 = NULL;

  // evaluate args
    
  double tmp,value1,value2,value3;

  if (narg == 1) {
    value1 = evaluate(arg1);
  } else if (narg == 2) {
    value1 = evaluate(arg1);
    value2 = evaluate(arg2);
  } else if (narg == 3) {
    value1 = evaluate(arg1);
    value2 = evaluate(arg2);
    value3 = evaluate(arg3);
  }
    
  if (strcmp(word,"sqrt") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    if (value1 < 0.0) 
      error->all("Sqrt of negative value in variable formula");
    argstack[nargstack++] = sqrt(value1);

  } else if (strcmp(word,"exp") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = exp(value1);
  } else if (strcmp(word,"ln") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    if (value1 <= 0.0) 
      error->all("Log of zero/negative value in variable formula");
    argstack[nargstack++] = log(value1);
  } else if (strcmp(word,"log") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    if (value1 <= 0.0) 
      error->all("Log of zero/negative value in variable formula");
    argstack[nargstack++] = log10(value1);

  } else if (strcmp(word,"sin") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = sin(value1);
  } else if (strcmp(word,"cos") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = cos(value1);
  } else if (strcmp(word,"tan") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = tan(value1);

  } else if (strcmp(word,"asin") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    if (value1 < -1.0 || value1 > 1.0) 
      error->all("Arcsin of invalid value in variable formula");
    argstack[nargstack++] = asin(value1);
  } else if (strcmp(word,"acos") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    if (value1 < -1.0 || value1 > 1.0) 
      error->all("Arccos of invalid value in variable formula");
    argstack[nargstack++] = acos(value1);
  } else if (strcmp(word,"atan") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = atan(value1);
  } else if (strcmp(word,"atan2") == 0) {
    if (narg != 2) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = atan2(value1,value2);

  } else if (strcmp(word,"random") == 0) {
    if (narg != 3) error->all("Invalid math function in variable formula");
    if (randomequal == NULL) {
      int seed = static_cast<int> (value3);
      if (seed <= 0) error->all("Invalid math function in variable formula");
      randomequal = new RanMars(oink,seed);
    }
    argstack[nargstack++] = randomequal->uniform()*(value2-value1) + value1;
  } else if (strcmp(word,"normal") == 0) {
    if (narg != 3) error->all("Invalid math function in variable formula");
    if (value2 < 0.0) 
      error->all("Invalid math function in variable formula");
    if (randomequal == NULL) {
      int seed = static_cast<int> (value3);
      if (seed <= 0) error->all("Invalid math function in variable formula");
      randomequal = new RanMars(oink,seed);
    }
    argstack[nargstack++] = value1 + value2*randomequal->gaussian();

  } else if (strcmp(word,"ceil") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = ceil(value1);

  } else if (strcmp(word,"floor") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = floor(value1);

  } else if (strcmp(word,"round") == 0) {
    if (narg != 1) error->all("Invalid math function in variable formula");
    argstack[nargstack++] = MYROUND(value1);
  }

  delete [] arg1;
  delete [] arg2;
  delete [] arg3;

  return 1;
}

/* ----------------------------------------------------------------------
   check if word matches a constant
   return 1 if yes, else 0
   customize by adding a constant: PI
------------------------------------------------------------------------- */

int Variable::is_constant(char *word)
{
  if (strcmp(word,"PI") == 0) return 1;
  return 0;
}

/* ----------------------------------------------------------------------
   process a constant in formula
   customize by adding a constant: PI
------------------------------------------------------------------------- */

double Variable::constant(char *word)
{
  if (strcmp(word,"PI") == 0) return PI;
  return 0.0;
}

/* ----------------------------------------------------------------------
   check if word matches a keyword
   return 1 if yes, else 0
   customize by adding a keyword: nprocs, time
------------------------------------------------------------------------- */

int Variable::is_keyword(char *word)
{
  if (strcmp(word,"nprocs") == 0) return 1;
  else if (strcmp(word,"time") == 0) return 1;
  return 0;
}

/* ----------------------------------------------------------------------
   process a keyword in formula
   customize by adding a keyword: nprocs, time
------------------------------------------------------------------------- */

double Variable::keyword(char *word)
{
  if (strcmp(word,"nprocs") == 0) {
    int nprocs;
    MPI_Comm_size(world,&nprocs);
    return 1.0*nprocs;
  } else if (strcmp(word,"time") == 0) {
    return input->deltatime;
  }
  return 0.0;
}
/* ----------------------------------------------------------------------
   read a floating point value from a string
   generate an error if not a legitimate floating point value
------------------------------------------------------------------------- */

double Variable::numeric(char *str)
{
  int n = strlen(str);
  for (int i = 0; i < n; i++) {
    if (isdigit(str[i])) continue;
    if (str[i] == '-' || str[i] == '+' || str[i] == '.') continue;
    if (str[i] == 'e' || str[i] == 'E') continue;
    error->all("Expected floating point parameter in variable definition");
  }

  return atof(str);
}

/* ----------------------------------------------------------------------
   read an integer value from a string
   generate an error if not a legitimate integer value
------------------------------------------------------------------------- */

int Variable::inumeric(char *str)
{
  int n = strlen(str);
  for (int i = 0; i < n; i++) {
    if (isdigit(str[i]) || str[i] == '-' || str[i] == '+') continue;
    error->all("Expected integer parameter in variable definition");
  }

  return atoi(str);
}

/* ----------------------------------------------------------------------
   recursive evaluation of string str
   called from "if" command in input script
   str is a boolean expression containing one or more items:
     number = 0.0, -5.45, 2.8e-4, ...
     math operation = (),x==y,x!=y,x<y,x<=y,x>y,x>=y,x&&y,x||y
------------------------------------------------------------------------- */

double Variable::evaluate_boolean(char *str)
{
  int op,opprevious;
  double value1,value2;
  char onechar;
  char *ptr;

  double argstack[MAXLEVEL];
  int opstack[MAXLEVEL];
  int nargstack = 0;
  int nopstack = 0;

  int i = 0;
  int expect = ARG;

  while (1) {
    onechar = str[i];
    
    // whitespace: just skip
    
    if (isspace(onechar)) i++;
    
    // ----------------
    // parentheses: recursively evaluate contents of parens
    // ----------------
    
    else if (onechar == '(') {
      if (expect == OP) error->all("Invalid Boolean syntax in if command");
      expect = OP;
      
      char *contents;
      i = find_matching_paren(str,i,contents);
      i++;
      
      // evaluate contents and push on stack
      
      argstack[nargstack++] = evaluate_boolean(contents);
      
      delete [] contents;
      
    // ----------------
    // number: push value onto stack
    // ----------------
      
    } else if (isdigit(onechar) || onechar == '.' || onechar == '-') {
      if (expect == OP) error->all("Invalid Boolean syntax in if command");
      expect = OP;
      
      // istop = end of number, including scientific notation
      
      int istart = i++;
      while (isdigit(str[i]) || str[i] == '.') i++;
      if (str[i] == 'e' || str[i] == 'E') {
	i++;
	if (str[i] == '+' || str[i] == '-') i++;
	while (isdigit(str[i])) i++;
      }
      int istop = i - 1;
      
      int n = istop - istart + 1;
      char *number = new char[n+1];
      strncpy(number,&str[istart],n);
      number[n] = '\0';
      
      argstack[nargstack++] = atof(number);
      
      delete [] number;
      
    // ----------------
    // Boolean operator, including end-of-string
    // ----------------
      
    } else if (strchr("<>=!&|\0",onechar)) {
      if (onechar == '=') {
	if (str[i+1] != '=') 
	  error->all("Invalid Boolean syntax in if command");
	op = EQ;
	i++;
      } else if (onechar == '!') {
	if (str[i+1] == '=') {
	  op = NE;
	  i++;
	} else op = NOT;
      } else if (onechar == '<') {
	if (str[i+1] != '=') op = LT;
	else {
	  op = LE;
	  i++;
	}
      } else if (onechar == '>') {
	if (str[i+1] != '=') op = GT;
	else {
	  op = GE;
	  i++;
	}
      } else if (onechar == '&') {
	if (str[i+1] != '&') 
	  error->all("Invalid Boolean syntax in if command");
	op = AND;
	i++;
      } else if (onechar == '|') {
	if (str[i+1] != '|') 
	  error->all("Invalid Boolean syntax in if command");
	op = OR;
	i++;
      } else op = DONE;
      
      i++;
      
      if (op == NOT && expect == ARG) {
	opstack[nopstack++] = op;
	continue;
      }

      if (expect == ARG) error->all("Invalid Boolean syntax in if command");
      expect = ARG;
      
      // evaluate stack as deep as possible while respecting precedence
      // before pushing current op onto stack

      while (nopstack && precedence[opstack[nopstack-1]] >= precedence[op]) {
	opprevious = opstack[--nopstack];
	
	value2 = argstack[--nargstack];
	if (opprevious != NOT) value1 = argstack[--nargstack];
	
	if (opprevious == NOT) {
	  if (value2 == 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == EQ) {
	  if (value1 == value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == NE) {
	  if (value1 != value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == LT) {
	  if (value1 < value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == LE) {
	  if (value1 <= value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == GT) {
	  if (value1 > value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == GE) {
	  if (value1 >= value2) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == AND) {
	  if (value1 != 0.0 && value2 != 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	} else if (opprevious == OR) {
	  if (value1 != 0.0 || value2 != 0.0) argstack[nargstack++] = 1.0;
	  else argstack[nargstack++] = 0.0;
	}
      }
      
      // if end-of-string, break out of entire formula evaluation loop
      
      if (op == DONE) break;
      
      // push current operation onto stack
      
      opstack[nopstack++] = op;

    } else error->all("Invalid Boolean syntax in if command");
  }

  if (nopstack) error->all("Invalid Boolean syntax in if command");
  if (nargstack != 1) error->all("Invalid Boolean syntax in if command");
  return argstack[0];
}
