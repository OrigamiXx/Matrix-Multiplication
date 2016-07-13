#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <math.h>
#include <assert.h>
#include "usp_bi.h"

using namespace std;

typedef long set_long;

// Macroed implementation of two sets as an long.
// Operates on two sets on universe = {0,1,2,...,30}.

#define TO_S2(a) (1L << (a))
#define TO_S3(a) (1L << ((a) + 32))

#define SETS_EMPTY(a) ((a) == 0L)
#define CREATE_FULL_INT(n) ((n) == 32 ? ~0 : ~((1 << 31) >> (31 - (n))))
#define CREATE_FULL(n) ((long)(CREATE_FULL_INT(n)) | (((long)CREATE_FULL_INT(n)) << 32))
#define CREATE_EMPTY() (0L)

#define COMPLEMENT(s,n) (CREATE_FULL(n) ^ s)

#define MEMBER_S2(s,a) (((s) & TO_S2(a)) != 0L)
#define MEMBER_S3(s,a) (((s) & TO_S3(a)) != 0L)

#define INSERT_S2(s,a) ((s) | TO_S2(a))
#define INSERT_S3(s,a) ((s) | TO_S3(a))

#define DELETE_S2(s,a) ((s) & ~(TO_S2(a)))
#define DELETE_S3(s,a) ((s) & ~(TO_S3(a)))

#define SET_ID(s) (INSERT_S3(s,31))
#define UNSET_ID(s) (DELETE_S3(s,31))
#define IS_ID(s) (MEMBER_S3(s,31))

void print_sets(set_long sets){

  printf("s2 = { ");
  for (int i = 0; i < 31; i++){

    if (MEMBER_S2(sets,i))
      printf("%d ",i);

  }
  printf("}, s3 = { ");
  for (int i = 0; i < 31; i++){

    if (MEMBER_S3(sets,i))
      printf("%d ",i);

  }
  printf("}\n");

}

bool valid_combination(int u_1, int u_2, int u_3, int k){
  
  for (int i = 0; i < k; i++){
    int count = 0;
    int d1 = u_1 % 3;
    int d2 = u_2 % 3;
    int d3 = u_3 % 3;
    u_1 = (u_1 - d1) / 3;
    u_2 = (u_2 - d2) / 3;
    u_3 = (u_3 - d3) / 3;
    
    if (d1 == 0) count++;
    if (d2 == 1) count++;
    if (d3 == 2) count++;
    if (count == 2) return true;
  }
  return false;
}


bool ident(vector<int> p, int s){ 
  for (int i = 0; i < s; i++){
    if (p[i] != i) return false;
  }
  return true;
}

bool check_usp_uni(puzzle_row U[], int s, int k){

  bool go1 = true;
  bool check_memo[s][s][s];
  
  for (int i = 0; i < s; i++){
    for (int j = 0; j < s; j++){
      for (int r = 0; r < s; r++){
	check_memo[i][j][r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }

  int c = 0;
  vector<int> p_2(s);
  vector<int> p_3(s);
  for (int i = 0; i < s; i++) p_2[i] = i;
  for (; go1 ; go1 = next_permutation(p_2.begin(),p_2.end())){
    int go2 = true;
    c++;
    for (int i = 0; i < s; i++) p_3[i] = i;
    for (; go2 ; go2 = next_permutation(p_3.begin(),p_3.end())){
      if (ident(p_2,s) && ident(p_3,s)) continue;

      bool found = false;
      for (int i = 0; i < s; i++){
	if (check_memo[i][p_2[i]][p_3[i]]) {
	  found = true;
	  break;
	}
      }
      if (!found)
	return false;
    }     
  }

  return true;
}


void printU(puzzle_row U[], int s, int k){
  for (int i =0; i < s; i++){
    int x = U[i];
    for (int j = 0; j < k; j++){
      printf("%d ",x % 3 + 1);
      x = (x - (x % 3)) / 3;
    }
    printf("\n");
  }
  printf("---\n");
}






// Generates front direction of search.
void find_witness_forward(int w, int k, map<set_long,bool> * memo, int i1, set_long sets, bool * RR, bool is_id){

  // Unset identity flag if last choices were distinct.
   if (!is_id)  
    sets = UNSET_ID(sets);

  map<set_long,bool>::const_iterator iter = memo -> find(sets);

  if (iter != memo -> end()){
    return;
  }

  if (i1 == (int)(floorf(w / 2.0))) {
    //print_sets(sets);
    memo -> insert(pair<set_long,bool>(sets,false)); // XXX - Value saved is meaningless.
    return;
  }

  for (int i2 = 0; i2 < w; i2++){
    if (MEMBER_S2(sets,i2)) 
      continue;
    for (int i3 = 0; i3 < w; i3++){
      if (MEMBER_S3(sets,i3)) 
	continue;
      if (RR[i1 * w * w + i2 * w + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets,i2),i3);     
      
      find_witness_forward(w,k,memo,i1+1,sets2,RR, is_id && i1 == i2 && i2 == i3);
    }
  }
  
  memo -> insert(pair<set_long,bool>(sets,false)); // XXX - Value saved is meaningless.
  return;
  
}


bool find_witness_reverse(int w, int k, map<set_long,bool> * memo, int i1, set_long sets, bool * RR, bool is_id, map<set_long,bool> * opposite){

  // Unset identity flag if last choices were distinct.
  if (!is_id) 
    sets = UNSET_ID(sets);
  
  map<set_long,bool>::const_iterator iter = memo -> find(sets);

  if (iter != memo -> end()){
    return iter->second;
  }

  if (i1 < (int)(floorf((w - 1)/2.0))) {
    //printf("w = %d i1 = %d\n", w,i1);
    //print_sets(sets);

    set_long sets_comp = COMPLEMENT(sets,w);
    //print_sets(sets_comp);
    bool found_pair = false;

    if (is_id) {
      sets_comp = UNSET_ID(sets_comp);
    } else {
      iter = opposite -> find(sets_comp);      
      if (iter != opposite -> end()){
	found_pair = true;
      }     
      sets_comp = SET_ID(sets_comp);
    }

    iter = opposite -> find(sets_comp);
    if (iter != opposite -> end()){
      found_pair = true;
    }

    memo -> insert(pair<set_long,bool>(sets,found_pair));

    if (found_pair){
      //fprintf(stderr,"FOUND MATCH!\n");
    }
    return found_pair;
  }

  for (int i2 = 0; i2 < w; i2++){
    if (MEMBER_S2(sets,i2)) 
      continue;
    for (int i3 = 0; i3 < w; i3++){
      if (MEMBER_S3(sets,i3)) 
	continue;
      if (RR[i1 * w * w + i2 * w + i3] == true) 
	continue;

      set_long sets2 = INSERT_S3(INSERT_S2(sets,i2),i3);     

      if (find_witness_reverse(w,k,memo,i1-1,sets2,RR,is_id && i1 == i2 && i2 == i3, opposite)){
	memo -> insert(pair<set_long,bool>(sets,true));
	
	return true;
      }
      
    }
  }

  memo -> insert(pair<set_long,bool>(sets,false));
  return false;
  
}

bool check_usp_bi(puzzle_row U[], int w, int k){
  
  bool RR[w * w * w];
  
  for (int i = 0; i < w; i++){
    for (int j = 0; j < w; j++){
      for (int r = 0; r < w; r++){
	RR[i * w * w + j * w + r] = valid_combination(U[i],U[j],U[r],k);
      }
    }
  }
  
  map<set_long,bool> forward_memo;
  map<set_long,bool> reverse_memo;
  
  find_witness_forward(w,k,&forward_memo,0,SET_ID(0L),RR,true);
  
  return !find_witness_reverse(w,k,&reverse_memo,w-1,SET_ID(0L),RR,true,&forward_memo);
  
}

map<string,void *> * cache[5];
int cache_size = -1;
puzzle_row local_max_row = -1;

string U_to_string(puzzle_row U[], int s){

  ostringstream oss;
  oss << U[0];
  for (int i = 1; i < s; i++)
    oss << "|" << U[i];
  return oss.str();

}

bool cache_lookup(puzzle_row U[], int s, int k){

  assert(s >= 1 && s <= 4);

  if (s == 1)
    return true;

  map<string,void*>::const_iterator iter = cache[s] -> find(U_to_string(U,s));

  return (iter != cache[s] -> end());

}

bool check(puzzle_row U[], int s, int k){
  
  if (cache_size >= s)
    return cache_lookup(U,s,k);
  else if (s >= 3)
    return check_usp_bi(U,s,k);
  else
    return check_usp_uni(U,s,k);

}


bool check2(puzzle_row r1, puzzle_row r2, int k){

  puzzle_row U[2];
  U[0] = r1;
  U[1] = r2;
  return check(U,2,k);

}

bool check3(puzzle_row r1, puzzle_row r2, puzzle_row r3, int k){

  puzzle_row U[3];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  return check(U,3,k);
  
}

bool check4(puzzle_row r1, puzzle_row r2, puzzle_row r3, puzzle_row r4, int k){
  
  puzzle_row U[4];
  U[0] = r1;
  U[1] = r2;
  U[2] = r3;
  U[3] = r4;
  return check(U,4,k);

}

void init_cache(int k, int s){
  
  assert(s <= 4);

  //printf("Creating USP cache.\n");

  local_max_row = (puzzle_row)pow(3,k);

  int curr_size = cache_size;
  cache_size = -1;
  for (int x = 2; x <= curr_size; x++){
    delete cache[x];
  }

  puzzle_row U[4];

  if (s >= 2){
    
    cache[2] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	if (check(U,2,k)) {
	  //cout << "Cached USP: " << U_to_string(U,2) << endl;
	  cache[2] -> insert(pair<string,void *>(U_to_string(U,2),NULL));
	}
      }
    }
    
  } 

  if (s >= 3) {
    
    cache[3] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < local_max_row; U[2]++){
	  if (check(U,3,k)) {
	    cache[3] -> insert(pair<string,void *>(U_to_string(U,3),NULL));
	  }
	}
      }	
    }
  }
  
  if (s >= 4){
    
    cache[4] = new map<string,void *>();
    
    for (U[0] = 0; U[0] < local_max_row; U[0]++){
      for (U[1] = U[0]+1; U[1] < local_max_row; U[1]++){
	for (U[2] = U[1]+1; U[2] < local_max_row; U[2]++){
	  for (U[3] = U[2]+1; U[3] < local_max_row; U[3]++){
	    if (check(U,4,k)) {
	      cache[4] -> insert(pair<string,void *>(U_to_string(U,4),NULL));
	    }
	  }
	}
      }
    }	
    
  }
  
  cache_size = s;

  //printf("Created USP cache up to size = %d\n",cache_size);
  
}
  
