
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <vector>

#include "puzzle.h"
#include "usp.h"
#include "heuristic.h"
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
    max_s = 0;
    max_k = 0;
  }
  
  Checker(checker_t checker, string name, int max_s, int max_k){
    this -> checker = checker;
    this -> name = name;
    this -> max_s = max_s;
    this -> max_k = max_k;
  }

  bool checkable(puzzle * p){

    bool res = true;
    if (max_s > 0)
      res = res && (p -> s <= max_s);
    if (max_k > 0)
      res = res && (p -> k <= max_k);

    return res;
    
  }
  
 private:
  int max_s;
  int max_k;

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

  bool success = true;
  int test_num = 0;
  
  while (!(G -> is_empty())){
    test_num++;

    puzzle * p = G -> next_puzzle();

    check_t results[checkers -> size()];
    bool checkable[checkers -> size()];

    unsigned int i = 0;
    for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
      if ((*it).checkable(p)) {
	checkable[i] = true;
	results[i] = ((*it).checker)(p);
      } else {
	checkable[i] = false;
      }
      i++;
    }

    bool found_IS_USP = false;
    bool found_NOT_USP = false;
    bool found_UNDET_USP = false;
    bool found_INVALID = false;
    bool found_uncheckable = false;
    for (i = 0; i < checkers -> size(); i++){
      if (!checkable[i]) 
	found_uncheckable = true;
      else if (results[i] == IS_USP)
	found_IS_USP = true;
      else if (results[i] == NOT_USP)
	found_NOT_USP = true;
      else if (results[i] == UNDET_USP)
	found_UNDET_USP = true;
      else
	assert(false == "Can't get here.");
    }

    bool consistent = !(found_IS_USP && found_NOT_USP);

    if (!consistent){
      success = false;
      cout << "Error: Test #"  << test_num << " Returns Inconsistent.\n";
      fprint_puzzle(output, p);
      cout << "\n";

      if (found_IS_USP){
	cout << "IS_USP:    ";
	i = 0;
	for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	  if (checkable[i] && results[i] == IS_USP)
	    cout << (*it).name << " ";
	  i++;
	}
	cout << "\n";
      }
      
      if (found_NOT_USP){
	cout << "NOT_USP:   ";
	i = 0;
	for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	  if (checkable[i] && results[i] == NOT_USP)
	    cout << (*it).name << " ";
	  i++;
	}
	cout << "\n";
      }
      
      if (found_UNDET_USP){
	cout << "UNDET_USP: ";
	i = 0;
	for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	  if (checkable[i] && results[i] == UNDET_USP)
	    cout << (*it).name << " ";
	  i++;
	}
	cout << "\n";
      }

      if (found_uncheckable){
      	cout << "Skipped:   ";
	i = 0;
	for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	  if (!checkable[i])
	    cout << (*it).name << " ";
	  i++;
	}
	cout << "\n";
      }
      
    }

    destroy_puzzle(p);
  }
  
  return success;
}


int main() {

  Checker c_uni(&check_usp_uni, "UNI", 3, 3);
  Checker c_bi(&check_usp_bi, "BI");
  Checker c_SAT(&check_SAT, "SAT");
  Checker c_MIP(&check_MIP, "MIP");
  Checker c_SAT_MIP(&check_SAT_MIP, "SAT_MIP");
  Checker c_full(&check, "Full");
  Checker h_greedy(&heuristic_greedy, "greedy");
  Checker h_random(&heuristic_random, "random");
  
  vector<Checker> checkers;

  checkers.push_back(c_uni);
  checkers.push_back(c_bi);
  checkers.push_back(c_SAT);
  checkers.push_back(c_MIP);
  checkers.push_back(c_SAT_MIP);
  checkers.push_back(c_full);
  checkers.push_back(h_random);
  checkers.push_back(h_greedy);
  
  
  Random_Generator G(4, 4, 100);
  
  cout << "--------------------------- Starting tests ----------------------------\n";
  
  run_test(&G, &checkers);

  cout << "--------------------------- Tests complete ----------------------------\n";
   
  return 0;
}
