
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <vector>

#include "puzzle.h"
#include "usp.h"
#include "timing.h"

#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"

using namespace std;

FILE * output = stdout;

class Checker {

 public:
  checker_t checker;
  string name;
  
  Checker(checker_t checker, string name){
    this -> checker = checker;
    this -> name = name;
  }

};

class Puzzle_Generator {

 public:
  virtual puzzle * next_puzzle(void) {};
  virtual bool is_empty(void) {};

};

class Random_Generator : public Puzzle_Generator {

 private:
  int s;
  int k;
  int count;
  
 public:
  Random_Generator(int s, int k, int count){
    this -> s = s;
    this -> k = k;
    this -> count = count;
  }
  
  puzzle * next_puzzle(void) {
    puzzle * p = create_puzzle(s, k);
    randomize_puzzle(p);
    count--;
    return p;
  }

  bool is_empty(void) {
    return (count <= 0);
  }

};

class File_Generator : public Puzzle_Generator {

 private:
  FILE * f;
    
 public:
  File_Generator(const char * fname){
    f = fopen(fname, "r");
  }

  puzzle * next_puzzle(void) {
    puzzle * p = create_next_puzzle_from_file(f);
    return p;
  }

  bool is_empty(void) {
    return (feof(f));
  }
  
};

  

bool run_test(Puzzle_Generator * G, vector<Checker> * checkers){

  while (!(G -> is_empty())){

    puzzle * p = G -> next_puzzle();

    check_t results[checkers -> size()];

    unsigned int i = 0;
    for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
      cout << (*it).name << endl;
      results[i] = ((*it).checker)(p);
      i++;
    }
    
    bool consistent = true;
    for (i = 0; i < checkers -> size(); i++){
      if (results[0] != results[i])
	consistent = false;
    }

    if (!consistent){
      cout << "Error: Test Inconsistent.\n";
      fprint_puzzle(output, p);
      cout << "\n";
      cout << "IS_USP:    ";
      i = 0;
      for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	if (results[i] == IS_USP)
	  cout << (*it).name;
	i++;
      }
      cout << "\n";
      cout << "NOT_USP:   ";
      i = 0;
      for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	if (results[i] == NOT_USP)
	  cout << (*it).name;
	i++;
      }
      cout << "\n";
      cout << "UNDET_USP: ";
      i = 0;
      for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	if (results[i] == UNDET_USP)
	  cout << (*it).name;
	i++;
      }
      cout << "\n";
     
    }

    destroy_puzzle(p);
  }
  
  
  
  return false;
}


int main() {

  Checker c_uni(&check_usp_uni, "UNI");
  Checker c_bi(&check_usp_bi, "BI");
  Checker c_SAT(&check_SAT, "SAT");
  Checker c_MIP(&check_MIP, "MIP");
  Checker c_SAT_MIP(&check_SAT_MIP, "SAT_MIP");
  Checker c_full(&check, "Full");
  
  vector<Checker> checkers;

  checkers.push_back(c_uni);
  checkers.push_back(c_bi);
  checkers.push_back(c_SAT);
  checkers.push_back(c_MIP);
  checkers.push_back(c_SAT_MIP);
  checkers.push_back(c_full);
  
  Random_Generator G(4, 4, 1);
  
  cout << "--------------------------- Starting tests ----------------------------\n";
  
  run_test(&G, &checkers);

  cout << "--------------------------- Tests complete ----------------------------\n";
   
  return 0;
}
