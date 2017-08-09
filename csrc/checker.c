/*
 * A simple consistency testing and benchmarking module. 
 *
 * Author: Matt.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <iomanip>
#include <math.h>

#include "puzzle.h"
#include "usp.h"
#include "heuristic.h"
#include "timing.h"

#include "3DM_to_SAT.h"
#include "3DM_to_MIP.h"

using namespace std;

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

  void reset(){
    times.clear();
  }
  
  bool checkable(puzzle * p){

    bool res = true;
    if (max_s > 0)
      res = res && (p -> s <= max_s);
    if (max_k > 0)
      res = res && (p -> k <= max_k);

    return res;
    
  }

  void add_time(double t, check_t res){
    times.push_back(pair<double,check_t>(t,res));
  }

  void static display_stats_header(){
 
    cout << "                     #         Total Time   Min (sec)    Max (sec)    Mean (sec)   Variance (%)" << endl;

  }
  
  void display_stats(){

    if (times.size() != 0) {
      int size[4] = {0};
      double total[4] = {0.0};
      double min[4] = {0.0};
      double max[4] = {0.0};
      double mean[4] = {0.0};
      double var[4] = {0.0};
      
      for (vector<pair<double, check_t> >::iterator it = times.begin() ; it != times.end(); ++it){
	double t = (*it).first;
	check_t res = (*it).second;
	total[3] += t;
	size[3]++;
	total[res] += t;
	size[res]++;
      }
      
      for (int i = 0; i < 4; i++){
	mean[i] = total[i] / size[i];
	min[i] = mean[i];
	max[i] = mean[i];
      }
      
      
      for (vector<pair<double, check_t> >::iterator it = times.begin() ; it != times.end(); ++it){
	double t = (*it).first;
	check_t res = (*it).second;
	var[3] += (t - mean[3]) * (t - mean[3]);
	var[res] += (t - mean[res]) * (t - mean[res]);
	if (t < min[res])
	  min[res] = t;
	if (t < min[3])
	  min[3] = t;
	if (t > max[res])
	  max[res] = t;
	if (t > max[3])
	  max[3] = t;
      }
      
      for (int i = 0; i < 4; i++){
	var[i] /= size[i];
	var[i] = sqrt(var[i]);
      }
      
      cout << "  " << name << endl;
      for (int i = 0; i < 4; i++){
	
	if (size[i] == 0) continue;
	
	if (i == 0)
	  cout << "    NOT_USP   ";
	else if (i == 1)
	  cout << "    IS_USP    ";
	else if (i == 2)
	  cout << "    UNDET_USP ";
	else
	  cout << "  Totals:     ";
	
	printf("%8d %18.8f %12.8f %12.8f %12.8f %10.2f\n",
	       size[i], total[i], min[i], max[i], mean[i], (var[i] / mean[i]) * 100.0);
	
      }
    } else {
      cout << "  " << name << " -- No puzzles checkable." << endl;
    }
     
  }
  

  
 private:
  int max_s;
  int max_k;
  vector<pair<double,check_t> > times;

};

class Puzzle_Tester {

 protected:
  double progress;
  string name;
  
 public:
  virtual puzzle * next_puzzle(void) {};
  virtual bool is_empty(void) {};

  bool run_test(vector<Checker> * checkers){

    bool success = true;
    int test_num = 0;
    
    while (!is_empty()){
      test_num++;
      
      puzzle * p = next_puzzle();
      //fprint_puzzle(stderr, p);
      
      check_t results[checkers -> size()];
      bool checkable[checkers -> size()];
      
      unsigned int i = 0;
      for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	if ((*it).checkable(p)) {
	  //cerr << (*it).name << endl;
	  checkable[i] = true;
	  double dt = 0.0;
	  results[i] = time_check((*it).checker, p, &dt);
	  (*it).add_time(dt, results[i]);
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
	cerr << "ERROR: Test #"  << test_num << " Returns Inconsistent.\n\n";
	fprint_puzzle(stderr, p);
	cerr << "\n";
	
	if (found_IS_USP){
	  cerr << "IS_USP:    ";
	  i = 0;
	  for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	    if (checkable[i] && results[i] == IS_USP)
	      cerr << (*it).name << " ";
	    i++;
	  }
	  cerr << "\n";
	}
	
	if (found_NOT_USP){
	  cerr << "NOT_USP:   ";
	  i = 0;
	  for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	    if (checkable[i] && results[i] == NOT_USP)
	      cerr << (*it).name << " ";
	    i++;
	  }
	  cerr << "\n";
	}
	
	if (found_UNDET_USP){
	  cerr << "UNDET_USP: ";
	  i = 0;
	  for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	    if (checkable[i] && results[i] == UNDET_USP)
	      cerr << (*it).name << " ";
	    i++;
	  }
	  cerr << "\n";
	}
	
	if (found_uncheckable){
	  cerr << "Skipped:   ";
	  i = 0;
	  for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
	    if (!checkable[i])
	      cerr << (*it).name << " ";
	    i++;
	  }
	  cerr << "\n";
	}
	cerr << "\n";
      }
      
      display_progress();
      
      destroy_puzzle(p);
    }

    cout << "\r" << "Test: " << name << " -> ";
    
    if (success)
      cout << "SUCCESS" << setw(80) << "" << endl;
    else
      cout << "FAILURE" << setw(80) << "" << endl;

    Checker::display_stats_header();
    for (vector<Checker>::iterator it = checkers -> begin() ; it != checkers -> end(); ++it){
      (*it).display_stats();
      cout << endl;
      (*it).reset();
    }
    
    return success;
  }


  
  void display_progress(){

    int percent = (int)(progress * 100.0);
    
    if (!is_empty()){
      cout << "\rTest: " << name <<" [";
      for (int i = 0; i < 50; i++){
	if (2*i <= percent)
	  cout << ">";
	else 
	  cout << "-";
      }
      cout << "] " << percent << "%";
      
      fflush(stdout);
    }
    
  }  
  
};

class Random_Tester : public Puzzle_Tester {

 private:
  int s;
  int k;
  int count;
  int completed;
  
 public:
  Random_Tester(string name, int s, int k, int count){
    this -> s = s;
    this -> k = k;
    this -> count = count;
    this -> name = name;
    completed = 0;
    progress = 0.0;
  }
  
  puzzle * next_puzzle(void) {
    puzzle * p = create_puzzle(s, k);
    randomize_puzzle(p);
    completed++;
    progress = completed / (double) count;
    return p;
  }

  bool is_empty(void) {
    return completed >= count;
  }

};

class File_Tester : public Puzzle_Tester {

 private:
  FILE * f;
  int lines;
  int lines_completed;
    
 public:
  File_Tester(string name, const char * fname){
    
    f = fopen(fname, "r");
    int max_buff = 1000;
    char line_buff[max_buff];
    lines = 0;
    while (fgets(line_buff, max_buff, f) != NULL)
      lines++;
    fclose(f);

    f = fopen(fname, "r");
    progress = 0.0;
    lines_completed = 0;
    this -> name = name;
  }

  puzzle * next_puzzle(void) {
    //printf("started next\n");
    int lines_read = 0;
    puzzle * p = create_next_puzzle_from_file(f, &lines_read);
    assert(p != NULL);
    lines_completed += lines_read;
    progress = lines_completed / (double) lines;
    //printf("completed next\n");
    return p;
  }

  bool is_empty(void) {
    return (feof(f));
  }
  
};

  



int main() {

  Checker c_full(&check, "Full");
  
  Checker c_uni(&check_usp_uni, "UNI", 6, 0);
  Checker c_bi(&check_usp_bi, "BI", 11, 0);
  Checker c_SAT(&check_SAT, "SAT", 35, 0);
  Checker c_MIP(&check_MIP, "MIP");
  Checker c_SAT_MIP(&check_SAT_MIP, "SAT_MIP");

  Checker h_greedy(&heuristic_greedy, "greedy", 31, 0);
  Checker h_random(&heuristic_random, "random", 31, 0);
  Checker h_row_pairs(&heuristic_row_pairs, "row_pairs");
  Checker h_row_triples(&heuristic_row_triples, "row_triples");
  Checker h_2d_matching(&heuristic_2d_matching, "2d_matching");

  // Experimental

  
  
  vector<Checker> checkers;

  checkers.push_back(c_full);
  
  checkers.push_back(c_uni);
  checkers.push_back(c_bi);
  checkers.push_back(c_SAT);
  checkers.push_back(c_MIP);
  checkers.push_back(c_SAT_MIP);

  checkers.push_back(h_random);
  checkers.push_back(h_greedy);
  checkers.push_back(h_row_pairs);
  checkers.push_back(h_row_triples);
  checkers.push_back(h_2d_matching);
  
  
  Random_Tester R1("Small Random1", 3, 3, 1000);
  Random_Tester R2("Small Random2", 5, 4, 10000);
  Random_Tester R3("Medium Random1", 7, 5, 100000);
  Random_Tester R4("Medium Random2", 12, 6, 10000);
  Random_Tester R5("Large Random1", 18, 8, 1000);
  Random_Tester R6("Large Random2", 30, 11, 100);
  Random_Tester R7("Huge Random1", 40, 13, 100);
  Random_Tester R8("Huge Random2", 70, 15, 100);
  Random_Tester R9("Yuge Random1", 80, 19, 10);
  
  File_Tester F1("File 8-5s", "test_data/8-5s.puz");
  File_Tester F2("File 13-6s", "test_data/13-6s.puz");
  File_Tester F3("File 13-6-usps", "test_data/13-6-usps.puz");
  
  cout << "--------------------------- Starting tests ----------------------------\n";
  
  R1.run_test(&checkers);
  R2.run_test(&checkers);
  R3.run_test(&checkers);
  R4.run_test(&checkers);
  R5.run_test(&checkers);
  R6.run_test(&checkers);
  R7.run_test(&checkers);
  R8.run_test(&checkers);
  R9.run_test(&checkers);
  F1.run_test(&checkers);
  F2.run_test(&checkers);
  F3.run_test(&checkers);

  cout << "--------------------------- Tests complete ----------------------------\n";
   
  return 0;
}
